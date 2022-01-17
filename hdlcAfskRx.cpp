#include "hdlcAfskRx.h"

HdlcAfskRx::HdlcAfskRx(HdlcFrameBuffer* rxBuffer) {
  _rxBuffer = rxBuffer;
}

void HdlcAfskRx::decodeSample(byte sampleVal) {
  int intAnVal = sampleVal - 127;

  _rxFilter[0][0] = _rxFilter[0][1];
  _rxFilter[0][1] = (_rxAnSamples[_histAnSampleIndex] * intAnVal) >> 2;
  _rxFilter[1][0] = _rxFilter[1][1];
  _rxFilter[1][1] = _rxFilter[0][0] + _rxFilter[0][1] + (_rxFilter[1][0] >> 1); 
  _rxAnSamples[_rxAnSampleIndex] = intAnVal;
  _histAnSampleIndex++;
  if (_histAnSampleIndex > MAX_RX_AN_SAMPLE_INDEX) {
    _histAnSampleIndex = 0;
  } 
  _rxAnSampleIndex++;
  if (_rxAnSampleIndex > MAX_RX_AN_SAMPLE_INDEX) {
    _rxAnSampleIndex = 0;
  }  
  _spaceMarkSamples <<= 1;
  _spaceMarkSamples |= (_rxFilter[1][1] > 0) ? 1 : 0; 
    
  if (
    (
      ( (_spaceMarkSamples)^(_spaceMarkSamples >> 2) ) & SPACE_MARK_TRANSISION_MASK
    ) == SPACE_MARK_TRANSISION_MASK
  ) {
    if (_currentPhase < HALF_WINDOW_SAMPLE_COUNT) { 
        _currentPhase += 1;
    } 
    else {
        _currentPhase -= 1;
    }
  }

  _currentPhase += RX_AN_SAMPLE_COUNT;
  if (_currentPhase >= WINDOW_SAMPLE_COUNT) {
    _currentPhase %= WINDOW_SAMPLE_COUNT;
    byte bits = _spaceMarkSamples & 0x07;
    setNextMark(
      (bits == 0x07 || // 111
       bits == 0x06 || // 110
       bits == 0x05 || // 101
       bits == 0x03)   // 011
    ); 
  }
}

void HdlcAfskRx::setNextMark(boolean spaceMark) {
  _prevSpace = _currentSpace;
  _currentSpace = spaceMark;

  boolean flagDetect = false;
  
  switch (stufState) {
    case STUF_NO_STUF:
      _afskShiftInReg >>= 1;
      _rxBitCount++;
      if (_rxBitCount > 8) {
        _rxBitCount = 1;
      }
      _afskShiftInReg |= (_prevSpace == _currentSpace) ? 0x80 : 0; 
      flagDetect = _afskShiftInReg == HDLC_FLAG;
      if ((_afskShiftInReg == STUF1) || (_afskShiftInReg == STUF2)) {
        stufState = STUF_SKIP; 
      }
      break;
    case STUF_SKIP:
      _afskShiftInReg |= (_prevSpace == _currentSpace) ? 0x80 : 0; 
      stufState = STUF_SKIP_CHECKS;
      break;
    case STUF_SKIP_CHECKS: 
      _afskShiftInReg >>= 1;
      _rxBitCount++;
      if (_rxBitCount > 8) {
        _rxBitCount = 1;
      }
      _afskShiftInReg |= (_prevSpace == _currentSpace) ? 0x80 : 0; 
      stufState = STUF_NO_STUF;
      break;
  }
 
  if (stufState == STUF_SKIP) {
    return; 
  }

  switch (_frameRxState) {
    case RXST_IDLE:
      if (flagDetect) {
        _rxBitCount = 0;
        _frameRxState = RXST_SYNC1;
      }
      break; 
    case RXST_SYNC1:
      if (_rxBitCount > 7) {
        if (flagDetect) {
          _rxBuffer->frameReset();
          _frameRxState = RXST_SYNC2;
        }
        else {
          _frameRxState = RXST_IDLE;
        }
      }
      break; 
    case RXST_SYNC2:
      if (_rxBitCount > 7) {
        if (!flagDetect) {
          _frameRxState = RXST_START;
		  receiving = true;
          if (!_rxBuffer->push(_afskShiftInReg)) {
            _frameRxState = RXST_IDLE;
          }
        }
      }
      break; 
    case RXST_START: 
      if (flagDetect) {
        _rxBitCount = 0;
        _frameRxState = RXST_SYNC1;
        receiving = false;
        _rxBuffer->frameEnd();
      }
      else if (_rxBitCount > 7) {
        if (!_rxBuffer->push(_afskShiftInReg)) {
          _frameRxState = RXST_IDLE;
          receiving = false;
        }
      }
      break; 
  }
}

