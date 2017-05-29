#ifndef HDLC_AFSK_TX_H
#define HDLC_AFSK_TX_H

#include "hdlcFramesBuffer.h"

#define AFSK_PIN_COUNT 4
#define AFSK_PIN_4 4
#define AFSK_PIN_5 5
#define AFSK_PIN_6 6
#define AFSK_PIN_7 7

#define SIN_TABLE_SIZE        48 
#define MARK_SAMPLE_DEG_STEP   6
#define SPACE_SAMPLE_DEG_STEP 11
#define MAX_BIT_SAMPLE_INDEX   7

#define ANALOG_ZERO      0x80
#define MASK_RESET_VALUE 0x01
#define MAX_BIT_INDEX       7

#define TX_FRAME_PREAMBLE 0
#define TX_FRAME_BEGIN    1
#define TX_FRAME_DATA     2
#define TX_FRAME_END      3

#define MAX_ONES_WITHOUT_STUFFING 4
#define DEFAULT_PREAMBLE_COUNT 75

class HdlcAfskTx {
  public:
    HdlcAfskTx(HdlcFrameBuffer* txBuffer);
    boolean loopbackTest=false;
    byte encodeSample();
    void start();
    boolean getReady();
    void setTxDelay(int val);
  private:
    HdlcFrameBuffer* _txBuffer;
    int _pins[AFSK_PIN_COUNT] = {AFSK_PIN_7, AFSK_PIN_6, AFSK_PIN_5, AFSK_PIN_4};
    int _n7_5deg = 0;
    int _n7_5degTest = 0;
    int _bitSampleCounter = 0;
    boolean _mark = true;
    boolean _runToZero = false;
    boolean _ready = true;
    byte _afskByte = HDLC_FLAG;
    byte _afskBitMask = MASK_RESET_VALUE;
    int frameTxState = TX_FRAME_PREAMBLE;
    int _afskBitPtr = 0;
    int _preambleCount = DEFAULT_PREAMBLE_COUNT;
    int _preampCountDown = DEFAULT_PREAMBLE_COUNT;
    boolean _enableStuffBit = false;
    boolean _stuffBit = false;
    int _oneCounter = 0;
    boolean getNextMark(boolean* current);
};

#endif
