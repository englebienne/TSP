#ifndef TSP_H
#define TSP_H

#include <Wire.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class definition
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define IRQ_PIN A12
#define TSP_RESET_PIN 14
#define NUM_RX 27
#define NUM_TX 19

class TSP {
     uint8_t lastAck;

     static const uint16_t buflen = 1024;
     uint8_t  msgBuffer[512];
     uint16_t cnt;
     uint16_t cal[NUM_RX][NUM_TX];
     uint8_t  mut[NUM_RX][NUM_TX];
     uint8_t  calibrating;
     
public:
     TSP()
          : gobbleTillCmdAck(0), locationInMsg(0), msgLen(0), currentCmd(0),
            gobbled(0), cnt(0)
          { };
     
     void init();
     void reset();

     bool     getData();
     
     byte     getRegisters(byte reg, byte size, byte *buffer);

     uint8_t  touchBuffer[0x40];
     uint8_t  printTouches();
     void     readInfo();
     void     sendCommand(uint8_t cmd, const uint8_t *data, uint8_t len);
     uint8_t  locationInMsg, msgLen, currentCmd, gobbleTillCmdAck, gobbled;

     void delayAndPoll(uint16_t usec);
     void interpretParam(uint8_t *buffer);
     void gobble(uint8_t cmd);
     void processByte (uint8_t b);
     bool interpretReply();
     void printVersion();
     void printParameter(uint16_t addr);
     void setParameter(uint16_t addr, uint32_t data, uint32_t mask);

     void baseline();
     void transmit();
};

#endif  /* TSP_H */
