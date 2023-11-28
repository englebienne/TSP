#ifndef TSP_H
#define TSP_H

#include <Wire.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class definition
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TSP {
     uint8_t lastAck;
public:
     TSP() {};
     bool init();
     void getRegisters(byte reg, byte size, byte *buffer);

     uint8_t touchBuffer[0x40];
     uint8_t printTouches();
     void readInfo();
     void sendCommand(uint8_t cmd, const uint8_t *data, uint8_t len);
     uint8_t locationInMsg, msgLen, currentCmd, gobbleTillCmdAck, gobbled;
     uint8_t buffer[256];
     
     void gobble(uint8_t cmd);
     void processByte (uint8_t b);
     void interpretReply();
     void printVersion();
     void printParameter(uint16_t addr);
     void setParameter(uint16_t addr, uint32_t data, uint32_t mask);
     void waitForAck(uint8_t cmd);
};

#endif  /* TSP_H */
