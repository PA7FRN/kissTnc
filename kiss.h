#ifndef KISS_H
#define KISS_H

#include "hdlcTnc.h"

#define FEND  0xC0
#define FESC  0xDB
#define TFEND 0xDC
#define TFESC 0xDD

#define KS_UNDEF    0
#define KS_GET_CMD  1
#define KS_GET_DATA 2
#define KS_ESCAPE   3

#define TIME_10_MS_UNITS 10

#define CMD_DATA_FRAME    0x00
#define CMD_TX_DELAY      0x01
#define CMD_P             0x02
#define CMD_SLOT_TIME     0x03
#define CMD_TX_TAIL       0x04
#define CMD_FULL_DUPLEX   0x05
#define CMD_SET_HARDWARE  0x06
#define MAX_KISS_CMD      0x07
#define CMD_UNDEF         0x80
#define MIN_KISS_CMD      0xFD
#define CMD_LOOPBACK_TEST 0xFE
#define CMD_RETURN        0xFF

#define LCD_ROW_COUNT  4
#define LCD_COL_COUNT 20
#define EMPTY_ROW "                    "

class Kiss {
  public:
    Kiss(
      int bdRate, 
      int rxIndicationPin,  int txIndicationPin, int rxOnlyPin, int pttPin, int lowLevelSelectPin, 
      int rxBufferSize, int txBufferSize
    );
    void begin();
    void kissTask();
  private:
    HdlcTnc* _hdlcTnc;
    int _baud;
    int  _kissInState = KS_UNDEF;
    byte _kissCommand = CMD_UNDEF;
    byte _tncParam;
    boolean _kissOutFrameEnd = true;
    boolean _readAddress = true;
    void processKissOutByte(byte newByte, boolean frameEnd);
    void processKissInByte(byte newByte);
    void storeDataByte(byte newByte);
    void sendCommandToTnc();
};

#endif
