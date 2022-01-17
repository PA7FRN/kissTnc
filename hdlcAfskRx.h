#ifndef HDLC_AFSK_RX_H
#define HDLC_AFSK_RX_H

#include "hdlcFramesBuffer.h"

#define STUF_NO_STUF     0
#define STUF_SKIP        1
#define STUF_SKIP_CHECKS 2

#define RXST_IDLE  0
#define RXST_SYNC1 1
#define RXST_SYNC2 2
#define RXST_START 3

#define STUF1 0x7c
#define STUF2 0x7d

#define RX_AN_SAMPLE_COUNT 8
#define MAX_RX_AN_SAMPLE_INDEX 7
#define WINDOW_SAMPLE_COUNT 64
#define HALF_WINDOW_SAMPLE_COUNT 32
#define SPACE_MARK_TRANSISION_MASK 0x03

class HdlcAfskRx {
  public:
    HdlcAfskRx(HdlcFrameBuffer* rxBuffer);
    void decodeSample(byte sampleVal);
    boolean receiving = false;
  private:
    HdlcFrameBuffer* _rxBuffer;
    int _rxFilter[2][2];
    int _rxAnSamples[RX_AN_SAMPLE_COUNT];
    int _rxAnSampleIndex   = 0;
    int _histAnSampleIndex = 4;
    byte _spaceMarkSamples = 0x00;
    int _currentPhase = 0;
    boolean _prevSpace = false;
    boolean _currentSpace = false;
    byte _afskShiftInReg = 0x00;
    int stufState = STUF_NO_STUF; 
    int  _rxBitCount = 0;
    int  _frameRxState = RXST_IDLE;
    void setNextMark(boolean spaceMark);
};

#endif
