#include "hdlcAfskTx.h"

const byte sinusTable[SIN_TABLE_SIZE] PROGMEM  = { //idx 0=0deg, 1=7.5deg, 2=15deg, 3=22.5deg, etc.
  0x80, 0x90, 0xa0, 0xb0, 0xbf, 0xcd, 0xda, 0xe5, 0xee, 0xf5, 0xfb, 0xfe, //   0.. 82.5deg
  0xff, 0xfe, 0xfb, 0xf5, 0xee, 0xe5, 0xda, 0xcd, 0xbf, 0xb0, 0xa0, 0x90, //  90..172.5deg
  0x80, 0x6f, 0x5f, 0x4f, 0x40, 0x32, 0x25, 0x1a, 0x11, 0x0a, 0x04, 0x01, // 180..262.5deg
  0x00, 0x01, 0x04, 0x0a, 0x11, 0x1a, 0x25, 0x32, 0x40, 0x4f, 0x5f, 0x6f  // 270..352.5deg
}; 

HdlcAfskTx::HdlcAfskTx(HdlcFrameBuffer* txBuffer) {
  _txBuffer = txBuffer;
  for (int idx=0; idx < AFSK_PIN_COUNT; idx++) {
    pinMode(_pins[idx], OUTPUT);
  }
}

byte HdlcAfskTx::encodeSample() {
  if (_ready) { return ANALOG_ZERO; }

  byte loopBackSample = ANALOG_ZERO;
  if (loopbackTest) {
    PORTD = (pgm_read_byte(&sinusTable[_n7_5degTest]) & AFSK_MASK) | (PORTD & NON_AFSK_MASK);
    _n7_5degTest += MARK_SAMPLE_DEG_STEP;
    if (_n7_5degTest >= SIN_TABLE_SIZE) {
      _n7_5degTest -= SIN_TABLE_SIZE;
    }
    loopBackSample = pgm_read_byte(&sinusTable[_n7_5deg]);
  } 
  else {
    PORTD = (pgm_read_byte(&sinusTable[_n7_5deg]) & AFSK_MASK) | (PORTD & NON_AFSK_MASK);
  }

  if (_mark) {
    _n7_5deg += MARK_SAMPLE_DEG_STEP;
  }
  else {
    _n7_5deg += SPACE_SAMPLE_DEG_STEP;
  }
  if (_n7_5deg >= SIN_TABLE_SIZE) {
    _n7_5deg -= SIN_TABLE_SIZE;
  }

  if (_runToZero) {
    if (_n7_5deg < SPACE_SAMPLE_DEG_STEP) {
      _n7_5deg = 0;
      _ready = true;
      _runToZero = false;
      _ready = true;
      PORTD = ANALOG_ZERO;
    }
  }
  else {
    _bitSampleCounter++;
    if (_bitSampleCounter > MAX_BIT_SAMPLE_INDEX) {
      _bitSampleCounter = 0;
      if (getNextMark(&_mark)) {
        _runToZero = true;
      }
    }
  }
  return loopBackSample;
}

void HdlcAfskTx::start() {
  if (_ready) {
    _mark = true;
    _afskByte = HDLC_FLAG;
    _afskBitMask = MASK_RESET_VALUE;
    _afskBitPtr = 0;
    _preampCountDown = _preambleCount;
    _enableStuffBit = false;
    _stuffBit = false;
    _oneCounter = 0;
    _ready = false;
  }
}

boolean HdlcAfskTx::getReady() {
  return _ready;
}

void HdlcAfskTx::setTxDelay(int val) {
  _preambleCount = val * 12 / 80;
}

boolean HdlcAfskTx::getNextMark(boolean* current) {
  if (_afskBitPtr == 0) {
    boolean frameEnd=false;
    switch (frameTxState) {
      case TX_FRAME_PREAMBLE:
        _afskByte = HDLC_FLAG;
        _enableStuffBit = false;
        _preampCountDown--;
        if (_preampCountDown < 1) {
          frameTxState = TX_FRAME_BEGIN;
        }
        break;
      case TX_FRAME_BEGIN:
        if (_txBuffer->more()) {
          _afskByte = HDLC_FLAG;
          _enableStuffBit = false;
          frameTxState = TX_FRAME_DATA;
        }
        else {
          frameTxState = TX_FRAME_PREAMBLE;
          return true;
        }
        break;
      case TX_FRAME_DATA:
        if (_txBuffer->pop(&_afskByte, &frameEnd)) {
          _enableStuffBit = true;
          if (frameEnd) {
            frameTxState = TX_FRAME_END;
          }
        }
        else {
            frameTxState = TX_FRAME_END;
        }
        break;
      case TX_FRAME_END:
        _afskByte = HDLC_FLAG;
        _enableStuffBit = false;
        frameTxState = TX_FRAME_BEGIN;
        break;
    }
  }

  boolean nextBit;
  if (_stuffBit) {
    nextBit = false;
    _stuffBit = false;
  }
  else {
    nextBit = (_afskByte & _afskBitMask) != 0;
    if (nextBit) {
      _oneCounter++;
      if (_oneCounter > MAX_ONES_WITHOUT_STUFFING) {
        _oneCounter = 0;
        _stuffBit = _enableStuffBit;
      }
    }
    else {
      _oneCounter = 0;
    }
    _afskBitPtr++;
    _afskBitMask <<= 1;
    if (_afskBitPtr > MAX_BIT_INDEX) {
      _afskBitPtr = 0;
      _afskBitMask = MASK_RESET_VALUE;
    }
  }

  if (!nextBit) {
    *current = !*current;
  }

  return false; 
}

