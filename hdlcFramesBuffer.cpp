#include "hdlcFramesBuffer.h"

HdlcFrameBuffer::HdlcFrameBuffer(boolean tx, HdlcCrcTable* crcTable, int bufferSize) {
  _tx = tx;
  _crc = new HdlcCrc(crcTable);
  _bufferData = new byte[bufferSize+6];
  _bufferData[0] = 0;
  _bufferData[1] = 0;
}

boolean HdlcFrameBuffer::push(byte in) {
  boolean result = spaceAvailable(); 
  if (spaceAvailable()) {
    _bufferData[_pushPosition] = in;
    incrementPushPosition();
    _pushFrameDataCounter++;
    _crc->calcFcs(in);
    return true;
  }
  else {
    return false;
  }
}

boolean HdlcFrameBuffer::pop(byte* out, boolean* frameEnd) {
  *frameEnd = false;
  if (more()) {
    *out = _bufferData[_popPosition];
    incrementPopPosition(1);
    _popFrameSize--;
    if (_popFrameSize < 1) {
      *frameEnd = true;
      if (!_tx) {
        incrementPopPosition(2);
      }
    }
    return true;
  }
  else {
    return false;
  }
}

boolean HdlcFrameBuffer::more() {
  if (_popFrameSize < 1) {
    _popFrameSize = (_bufferData[_popFrameSizeHPos] << 8) + _bufferData[_popFrameSizeLPos];
    if (_popFrameSize > 0) {
      _bufferData[_popFrameSizeHPos] = 0;
      _bufferData[_popFrameSizeLPos] = 0;
      _popFrameSizeHPos += _popFrameSize + 2;
      _popFrameSizeLPos += _popFrameSize + 2;
      if (_popFrameSizeHPos >= BUFFER_SIZE) {
        _popFrameSizeHPos -= BUFFER_SIZE;
      }
      if (_popFrameSizeLPos >= BUFFER_SIZE) {
        _popFrameSizeLPos -= BUFFER_SIZE;
      }
      incrementPopPosition(2);
      if (!_tx) {
        _popFrameSize -=2;
      }
    }
  }
  return (_popFrameSize > 0);
}

void HdlcFrameBuffer::frameReset(){
  _pushPosition = _pushFrameSizeLPos;
  incrementPushPosition();
  _pushFrameDataCounter = 0;
  _crc->resetFcs();
}

void HdlcFrameBuffer::frameEnd() {
  if (spaceAvailable()) {
    if (_tx)  {
      byte fcsL = _crc->getFcsL();
      byte fcsH = _crc->getFcsH();
      push(fcsL); 
      push(fcsH);

      startNewFrame();
    }
    else {
      if(_crc->checkFcs()) {
        startNewFrame();
      }
      else {
        frameReset();
      }
    }
  }
  else {
    frameReset();
  }
}

void HdlcFrameBuffer::startNewFrame() {
  _bufferData[_pushFrameSizeHPos] = (_pushFrameDataCounter >> 8) & 0xff;
  _bufferData[_pushFrameSizeLPos] = (_pushFrameDataCounter & 0xff);
  _pushFrameDataCounter = 0;

  _pushFrameSizeHPos = _pushPosition;
  incrementPushPosition();

  _pushFrameSizeLPos = _pushPosition;
  incrementPushPosition();

  _bufferData[_pushFrameSizeHPos] = 0;
  _bufferData[_pushFrameSizeLPos] = 0; 
  _crc->resetFcs();
}

void HdlcFrameBuffer::incrementPushPosition() {
	_pushPosition++;
	if (_pushPosition >= BUFFER_SIZE) {
	  _pushPosition=0;
	}
}

void HdlcFrameBuffer::incrementPopPosition(int count) {
  _popPosition += count;
  if (_popPosition >= BUFFER_SIZE) {
    _popPosition -= BUFFER_SIZE;
  }
}

boolean HdlcFrameBuffer::spaceAvailable() {
  int result = _popPosition - _pushPosition;
  if (result <= 0) {
    result += BUFFER_SIZE;
  }
  return result>4;
}

