#ifndef HDLC_CRC_TABLE_H
#define HDLC_CRC_TABLE_H

#include "Arduino.h"

#define CRC_RESET_VAL 0xFFFF

class HdlcCrcTable {
  public:
    HdlcCrcTable();
    uint16_t getValue(byte crcTableIndex);
  private:
};

#endif
