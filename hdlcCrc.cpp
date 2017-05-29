#include "hdlcCrc.h"

HdlcCrc::HdlcCrc(HdlcCrcTable* crcTable) {
  _crcTable = crcTable;
}

void HdlcCrc::resetFcs() {
  _fcs = CRC_RESET_VAL;
  _calcFcs[0] = _calcFcs[1];
  _calcFcs[1] = _fcs;
  _count = 0;
}

void HdlcCrc::calcFcs(byte newByte) {
  _fcs = (_fcs >> 8) ^ _crcTable->getValue((_fcs ^ newByte) & 0xff);
  _rxFcs[0] = _rxFcs[1];
  _rxFcs[1] = newByte;
  _calcFcs[0] = _calcFcs[1];
  _calcFcs[1] = _calcFcs[2];
  _calcFcs[2] = _fcs;
  _count++;
}

byte HdlcCrc::getFcsL() {
    return (_fcs & 0xff) ^ 0xff; 
}

byte HdlcCrc::getFcsH() {
    return (_fcs >> 8) ^ 0xff; 
}

boolean HdlcCrc::checkFcs() {
  boolean result = false;
  byte calcFcsL = (_calcFcs[0] & 0xff) ^ 0xff;
  byte calcFcsH = (_calcFcs[0] >> 8) ^ 0xff;

  if (_count > 2) {
    if (_rxFcs[0] == calcFcsL) {
      if (_rxFcs[1] == calcFcsH) {
        result = true;
      }
    }
  }
  return result;
}

