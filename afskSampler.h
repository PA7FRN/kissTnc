#ifndef AFSK_SAMPLER_H
#define AFSK_SAMPLER_H

#include "hdlcAfskTx.h"
#include "hdlcAfskRx.h"

class AfskSampler {
  public:
    AfskSampler(HdlcAfskRx* afskRx, HdlcAfskTx* afskTx);
  private:
};

#endif
