# KissTnc #

## Description ##
KissTnc is a C++ implementation of the TNC software for Arduino. It can be used for AX,25 / packet radio as well as for APRS applications. It is tested on the Arduino Nano and is compatible with the MicroAPRS. See http://unsigned.io/projects/microaprs/ for more information on MicroAPRS. The sketch and all used cpp and header files can be opened in the Arduino IDE.

## Hardware ##
Hardware can be made around a the Nano or other Arduino board, using the schematic added to this repository.
Default following pins are used:
 RX LED pin 10 
 TX LED pin 9
 PTT control pin 3
 The used analog input is A0
 The four pins for generating the AFSK modulation signal are pin 4, 5, 6 and 7.

The PTT and LED pins can be changed in the sketch by editing the defines in KissTnc INO.

It is advised to use the squelch on the radio to have a good carry detection.

## KISS Protocol ##
KissTnc is compatible with the KISS protocol as described at:
http://www.ax25.net/kiss.aspx

The foloowing KISS commands are implemented:
*   CMD_DATA_FRAME 0
*   CMD_TX_DELAY 
*   CMD_P 
*   CMD_SLOT_TIME
*   CMD_FULL_DUPLEX
	
For testing purpose an additional command is implemented:
*   CMD_LOOPBACK_TEST With enabling this function, packets transmitted from the host are routed back to the receiving part of the software. Instead of transmitting the packets, a constant 1200Hz tone is audible at the modulation output. After any change of the code, the software can be tested by doing a loopback test. The packet returned back to the host must be the same as the original ones. The signal set on the modulation output must be 1200Hz (+/-1). If this is not the case the ISR is not returning on time.

Following is not implemented:
*   CMD_TX_TAIL (obsolete)
*   CMD_SET_HARDWARE
*   CMD_RETURN
