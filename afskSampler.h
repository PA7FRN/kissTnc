#ifndef AFSK_SAMPLER_H
#define AFSK_SAMPLER_H

#include "hdlcAfskTx.h"
#include "hdlcAfskRx.h"

class AfskSampler {
  public:
    AfskSampler(int lowLevelSelectPin, HdlcAfskRx* afskRx, HdlcAfskTx* afskTx);
  private:
};

#endif
