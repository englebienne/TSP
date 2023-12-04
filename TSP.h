#ifndef TSP_H
#define TSP_H

#include <Wire.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class definition
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define IRQ_PIN A12
#define TSP_RESET_PIN A6

class TSP {
     uint8_t lastAck;

     // Circular buffer, originally to read data in ISR and process in main thread.
     // Now splits reading from interpretation
     static const uint16_t buflen = 1024;
     uint8_t circBuffer[buflen], msgBuffer[512];
     uint16_t wrIdx,             // Index at which to write
          rdIdx;                 // Index at which to read
     uint16_t cnt;
     void incIdx(uint16_t &i) { Serial.printf(" INCIDX(%d) ",i); i = (i+1) % buflen; }
     bool streamAvailable;      // Flag set in ISR
     
public:
     TSP()
          : gobbleTillCmdAck(0), locationInMsg(0), msgLen(0), currentCmd(0),
            gobbled(0), wrIdx(0), rdIdx(0),streamAvailable(true), cnt(0)
          { };
     
     void init();
     void reset();

     void handleInterrupt();
     void getData();
     
     byte getRegisters(byte reg, byte size, byte *buffer);

     uint8_t touchBuffer[0x40];
     uint8_t printTouches();
     void readInfo();
     void sendCommand(uint8_t cmd, const uint8_t *data, uint8_t len);
     uint8_t locationInMsg, msgLen, currentCmd, gobbleTillCmdAck, gobbled;

     void delayAndPoll(uint16_t usec);
     void interpretParam(uint8_t *buffer);
     void gobble(uint8_t cmd);
     void processByte (uint8_t b);
     void interpretReply();
     void printVersion();
     void printParameter(uint16_t addr);
     void setParameter(uint16_t addr, uint32_t data, uint32_t mask);
};

#endif  /* TSP_H */
