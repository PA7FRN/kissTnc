#include "kiss.h"

Kiss::Kiss(int baud, int rxIndicationPin, int pttPin, int rxBufferSize, int txBufferSize) {
  _baud = baud;
  _hdlcTnc = new HdlcTnc(rxIndicationPin, pttPin, rxBufferSize, txBufferSize);
}

void Kiss::begin() {
  Serial.begin(_baud);
}

void Kiss::kissTask() {
  _hdlcTnc->hdlcTask();
  if (Serial.available() > 0) {
    processKissInByte(Serial.read());
  }

  boolean frameEnd;
  byte rxByte;
  if (_hdlcTnc->getRxByte(&rxByte, &frameEnd)) {
    processKissOutByte(rxByte, frameEnd);
  }
}

void Kiss::processKissOutByte(byte newByte, boolean frameEnd) {
  if (_kissOutFrameEnd) {
    Serial.write(FEND);
    Serial.write(CMD_DATA_FRAME);
    _kissOutFrameEnd = false;
  }
  if (newByte == FEND) {
    Serial.write(FESC);
    Serial.write(TFEND);
  }
  else if (newByte == FESC) {
    Serial.write(FESC);
    Serial.write(TFESC);
  }
  else {
    Serial.write(newByte);
  }
  if (frameEnd) {
    Serial.write(FEND);
    _kissOutFrameEnd = true;
  }
}

void Kiss::processKissInByte(byte newByte) {
  switch (_kissInState) {
    case KS_UNDEF:
      if (newByte == FEND) {
        _kissCommand = CMD_UNDEF;
        _kissInState = KS_GET_CMD;
      }
      break;
    case KS_GET_CMD:
      if (newByte != FEND) {
        if ((newByte < MAX_KISS_CMD) || (newByte > MIN_KISS_CMD)) {
          _kissCommand = newByte;
          _kissInState = KS_GET_DATA;
        }
        else {
          _kissCommand = CMD_UNDEF;
          _kissInState = KS_UNDEF;
        }
      }
      break;
    case KS_GET_DATA:
      if (newByte == FESC) {
        _kissInState = KS_ESCAPE;
      }
      else if (newByte == FEND) {
        sendCommandToTnc();
        _kissCommand = CMD_UNDEF;
        _kissInState = KS_GET_CMD;
      }
      else {
        storeDataByte(newByte);
      }
      break;
    case KS_ESCAPE:
      if (newByte == TFEND) {
        storeDataByte(FEND); 
        _kissInState = KS_GET_DATA;
      }
      else if (newByte == TFESC) {
        storeDataByte(FESC); 
        _kissInState = KS_GET_DATA;
      }
      else {
        _hdlcTnc->resetTxFrame();
        _kissCommand = CMD_UNDEF;
        _kissInState = KS_UNDEF;
      }
      break;
  }
}

void Kiss::storeDataByte(byte newByte) {
  if (_kissCommand == CMD_DATA_FRAME) {
    _hdlcTnc->addByteToTxBuf(newByte);
  }
  else {
    _tncParam = newByte;
  }
}

void Kiss::sendCommandToTnc() {
  switch (_kissCommand) {
    case CMD_DATA_FRAME:
      _hdlcTnc->addFrameEndToTxBuf();
      break;
    case CMD_TX_DELAY:
      _hdlcTnc->setTxDelay(_tncParam * TIME_10_MS_UNITS);
      break;
    case CMD_P:
      _hdlcTnc->setPersistence(_tncParam);
      break;
    case CMD_SLOT_TIME:
      _hdlcTnc->setSlotTime(_tncParam * TIME_10_MS_UNITS);
      break;
    case CMD_TX_TAIL:
      //obsolete
      break;
    case CMD_FULL_DUPLEX:
      _hdlcTnc->setFullDuplex(_tncParam != 0);
      break;
    case CMD_SET_HARDWARE:
      //not implemented
      break;
    case CMD_LOOPBACK_TEST:
      _hdlcTnc->setLoopbackTest(_tncParam > 0);
      break;
    case CMD_RETURN:
      //not implemented
      break;
  }
}

