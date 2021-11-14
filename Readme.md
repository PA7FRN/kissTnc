# KissTnc #

## Description ##

KissTnc is a C++ implementation of the TNC software for Arduino. It can be used for AX.25 / packet radio as well as for APRS applications. It is tested on the Arduino Nano and is compatible with the MicroAPRS. See http://unsigned.io/projects/microaprs/ for more information on MicroAPRS. The sketch and all used cpp and header files can be opened in the Arduino IDE.

## Hardware ##

Hardware can be made around the Nano or other Arduino board, using the schematic added to this repository.
By default the following pins are used:

- RX LED pin D10
- TX LED pin D9
- RX only input D8
- PTT control pin D3
- Low level Input select pin D2
- The used analog input is A0
- The four pins for generating the AFSK modulation signal are pin D4, D5, D6 and D7.

The PTT, LED, RX only and level selection pins can be changed in the sketch by editing the defines in KissTnc.ino.

It is advised to use the squelch on the radio to have a good carry detection.
If the audio level from the transceiver is low (below 1V peak-to-peak), the TNC can be set in the low level mode. To run in low level mode, connect D2 to 5V, and connect R10 to Ref instead of 5V.

Rx only mode can be activated by connecting the RX only input (pin D9) to GND via a switch.

## KISS Protocol ##

KissTnc is compatible with the KISS protocol as described at:
<http://www.ax25.net/kiss.aspx>

The following KISS commands are implemented:

- CMD_DATA_FRAME (0x00)
- CMD_TX_DELAY (0x01)
- CMD_P (0x02)
- CMD_SLOT_TIME (0x03)
- CMD_FULL_DUPLEX (0x05)

For testing purpose an additional command is implemented:

- CMD_LOOPBACK_TEST (0xfe)

When enabling this function, packets transmitted from the host are routed back to the receiving part of the software. Instead of transmitting the packets, a constant 1200Hz tone is audible at the modulation output. After any change of the code, the software can be tested by doing a loop back test. The packet returned back to the host must be the same as the original ones. The signal set on the modulation output must be a stable 1200Hz (+/-1) tone. If this frequency is not 1200Hz or is not stable, the ISR is not returning on time.

Following is not implemented:

- CMD_TX_TAIL (0x04) (obsolete)
- CMD_SET_HARDWARE (0x06)
- CMD_RETURN (0xff)
