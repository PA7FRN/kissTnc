#include "kiss.h"

#define KISS_BD 9600
#define RX_LED /*10*/ 13 // for PI4RAZ APRS_iGate
#define TX_LED   9
#define RX_ONLY /*8*/ 10 // for PI4RAZ APRS_iGate (not used)
#define PTT_PIN  3
#define LOW_LEVEL_SELECT 2
#define RX_BUFFER_SIZE 500
#define TX_BUFFER_SIZE 500

Kiss kiss(
  KISS_BD, 
  RX_LED, TX_LED, RX_ONLY, PTT_PIN, LOW_LEVEL_SELECT, 
  RX_BUFFER_SIZE, TX_BUFFER_SIZE
);

void setup() {
  kiss.begin();
}

void loop() {
  kiss.kissTask();
}
