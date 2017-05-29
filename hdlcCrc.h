#ifndef HDLC_CRC_H
#define HDLC_CRC_H

#include "hdlcCrcTable.h"

#define CRC_RESET_VAL 0xFFFF

class HdlcCrc {
  public:
    HdlcCrc(HdlcCrcTable* crcTable);
    void resetFcs();
	  void calcFcs(byte newByte);
    byte getFcsL();
    byte getFcsH();
    boolean checkFcs();
  private:
    HdlcCrcTable* _crcTable;
    uint16_t _fcs = CRC_RESET_VAL;
    uint16_t _calcFcs[3];
    byte     _rxFcs[2];
    int  _count = 0;
};

#endif
