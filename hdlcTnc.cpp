#include "hdlcTnc.h"

HdlcTnc::HdlcTnc(
  int rxIndicationPin, int txIndicationPin, int rxOnlyPin, int pttPin, 
  int lowLevelSelectPin, int rxBufferSize, int txBufferSize
) {
  _crcTable = new HdlcCrcTable();
  _rxBuffer = new HdlcFrameBuffer(false, _crcTable, rxBufferSize);
  _txBuffer = new HdlcFrameBuffer(true , _crcTable, txBufferSize);
  _afskRx   = new HdlcAfskRx(_rxBuffer);
  _afskTx   = new HdlcAfskTx(_txBuffer);
  _afskSampler = new AfskSampler(lowLevelSelectPin, _afskRx, _afskTx);
  if (rxIndicationPin > 0 ) {
    pinMode(rxIndicationPin, OUTPUT);
    _rxIndicationPin = rxIndicationPin;
  }
  if (txIndicationPin > 0 ) {
    pinMode(txIndicationPin, OUTPUT);
    _txIndicationPin = txIndicationPin;
  }
  if (rxOnlyPin > 0 ) {
    pinMode(rxOnlyPin, INPUT_PULLUP);
    _rxOnlyPin = rxOnlyPin;
  }
  if (pttPin > 0 ) {
    pinMode(pttPin, OUTPUT);
    _pttPin = pttPin;
  }
}

void HdlcTnc::hdlcTask() {
  if (_afskRx->receiving) {
    digitalWrite(_rxIndicationPin, HIGH);
  }
  else {
    digitalWrite(_rxIndicationPin, LOW);
  }

  switch(_hdlcState) {
    case HS_IDLE:
      if (_txBuffer->more()) {
        if (_fullDuplex) {
          setPtt(true);
          _afskTx->start();
          _hdlcState = HS_TX_FRAME;
        }
        else {
          if (_afskRx->receiving) {
            _hdlcState = HS_WAIT_FOR_FREE_CHANNEL;
          }
          else {
            setPtt(true);
            _afskTx->start();
            _hdlcState = HS_TX_FRAME;
          }
        }
      }
      break;
    case HS_WAIT_FOR_FREE_CHANNEL:
      if (!_afskRx->receiving) {
        if (random(RANDOM_VAL_LIMIT) > _persistence) {
          _randomWaitTimeOutTime = millis() + _slotTime;
          _hdlcState = HS_RANDOM_WAIT;
        }
        else {
          setPtt(true);
          _afskTx->start();
          _hdlcState = HS_TX_FRAME;
        }
      }
      break;
    case HS_RANDOM_WAIT:
      if (_afskRx->receiving) {
        _hdlcState = HS_WAIT_FOR_FREE_CHANNEL;
      }
      else {
        if (millis() > _randomWaitTimeOutTime) { 
          setPtt(true);
          _afskTx->start();
          _hdlcState = HS_TX_FRAME;
        }
      }
      break;
    case HS_TX_FRAME:
      if (_afskTx->getReady()) {
        setPtt(false);
        _hdlcState = HS_IDLE;
      }
      break;
  }
}

void HdlcTnc::addByteToTxBuf(byte newByte) {
  if (digitalRead(_rxOnlyPin) == HIGH) {
    _txBuffer->push(newByte);
  }
}

void HdlcTnc::addFrameEndToTxBuf() {
  if (digitalRead(_rxOnlyPin) == HIGH) {
    _txBuffer->frameEnd();
  }
}

void HdlcTnc::resetTxFrame() {
  _txBuffer->frameReset();
}

boolean HdlcTnc::getRxByte(byte* rxByte, boolean* frameEnd) {
  return _rxBuffer->pop(rxByte, frameEnd);
}

void HdlcTnc::setTxDelay(int val) {
  _afskTx->setTxDelay(val);
}

void HdlcTnc::setPersistence(byte val) {
  _persistence = val;
}

void HdlcTnc::setSlotTime(int val) {
  _slotTime = val;
}

void HdlcTnc::setFullDuplex(boolean val) {
  _fullDuplex = val;
}

void HdlcTnc::setLoopbackTest(boolean on) {
  _afskTx->loopbackTest = on;
}

void HdlcTnc::setPtt(boolean on) {
  if (on) {
    digitalWrite(_pttPin, HIGH);
    digitalWrite(_txIndicationPin, HIGH);
  }
  else {
    digitalWrite(_pttPin, LOW);
    digitalWrite(_txIndicationPin, LOW);
  }
}

