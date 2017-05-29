#ifndef HDLC_FRAMES_BUFFER_H
#define HDLC_FRAMES_BUFFER_H

#include "hdlcCrc.h"

#define HDLC_FLAG  0x7e

#define BUFFER_SIZE 500

class HdlcFrameBuffer {
  public:
    HdlcFrameBuffer(boolean tx, HdlcCrcTable* crcTable, int bufferSize);
    boolean push(byte in);
    boolean pop(byte* out, boolean* frameEnd);
    boolean more();
    void frameReset(); 
    void frameEnd();
  private:
    HdlcCrc* _crc;
 // byte _bufferData[BUFFER_SIZE];
    byte* _bufferData;
    boolean _tx;
    int _pushFrameSizeHPos = 0;
    int _pushFrameSizeLPos = 1;
    int _pushPosition      = 2;
    int _pushFrameDataCounter = 0;
    int _popFrameSizeHPos = 0;
    int _popFrameSizeLPos = 1;
    int _popPosition      = 0;
    int _popFrameSize = 0;
    void startNewFrame();
    void incrementPushPosition();
    void incrementPopPosition(int count);
    boolean spaceAvailable();
};

#endif
