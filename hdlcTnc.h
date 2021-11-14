#ifndef HDLC_TNC_H
#define HDLC_TNC_H

#include "hdlcAfskRx.h"
#include "hdlcAfskTx.h"
#include "afskSampler.h"

#define DEFAULT_PERSISTENCE 63
#define DEFAULT_SLOT_TIME 100

#define RANDOM_VAL_LIMIT 256

#define HS_IDLE 0
#define HS_WAIT_FOR_FREE_CHANNEL 1
#define HS_RANDOM_WAIT 2
#define HS_TX_FRAME 3

class HdlcTnc {
  public:
    HdlcTnc(
      int rxIndicationPin, int txIndicationPin, int rxOnlyPin, int pttPin, 
      int lowLevelSelectPin, int rxBufferSize, int txBufferSize
    );
    void hdlcTask();
    void addByteToTxBuf(byte newByte);
    void addFrameEndToTxBuf();
    void resetTxFrame();
    boolean getRxByte(byte* rxByte, boolean* frameEnd);
    void setTxDelay(int val);
    void setPersistence(byte val);
    void setSlotTime(int val);
    void setFullDuplex(boolean val);
    void setLoopbackTest(boolean On);
  private:
    HdlcCrcTable* _crcTable;
    HdlcFrameBuffer* _rxBuffer;
    HdlcFrameBuffer* _txBuffer;
    HdlcAfskRx* _afskRx;
    HdlcAfskTx* _afskTx;
    AfskSampler* _afskSampler;
    int _rxIndicationPin = 0;
    int _txIndicationPin = 0;
    int _pttPin = 0;
    int _rxOnlyPin = 0;
    byte _persistence = DEFAULT_PERSISTENCE;
    int _slotTime = DEFAULT_SLOT_TIME;
    boolean _fullDuplex = false;
    int _hdlcState = HS_IDLE;
    unsigned long _randomWaitTimeOutTime = 0;
    void setPtt(boolean on);
};

#endif
