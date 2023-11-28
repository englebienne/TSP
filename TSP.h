#ifndef TSP_H
#define TSP_H

#include <Wire.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Defines
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define I2C_ADDR (uint8_t)0x25

#define REP_AdcDbg 0x60
#define REP_VER 0xFB
#define REP_ACK 0xF0
#define REP_PARAM_READ 0xCF
#define CMD_VER 0xFF
#define REP_TRACE 0x90
#define REP_GESTIC 0xFD

const char* SPACE = " ";

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class definition
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TSP {
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
};

#endif  /* TSP_H */
