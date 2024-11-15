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

#define STRH(x) #x
#define STR(x) STRH(x)

class TSP {
     uint8_t lastAck;
     uint16_t lastNVDM, lastNVAM;
     bool needReset;

     static const uint16_t buflen = 1024;
     uint8_t  msgBuffer[512];
     uint16_t cal[NUM_RX][NUM_TX];
     uint8_t  mut[NUM_RX][NUM_TX];
     uint8_t  calibrating;
     uint16_t cnt;
     void (*framehandler) (uint8_t *frame, unsigned len);
     
public:
     uint8_t  locationInMsg, msgLen, currentCmd, gobbleTillCmdAck, gobbled;
public:     

     TSP()
          : cnt(0), locationInMsg(0), msgLen(0), currentCmd(0),gobbleTillCmdAck(0), 
            gobbled(0)
          { };
     
     void init();
     void reset();

     bool     getData();
     
     byte     getRegisters(byte reg, byte size, byte *buffer);

     uint8_t  touchBuffer[0x40];
     uint8_t  printTouches();
     void     readInfo();
     void     sendCommand(uint8_t cmd, const uint8_t *data, uint8_t len);

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

     const char *getVersion() const { return "V. 2.1 TEST"; }
     const char *getResolution() const { return "RX=" STR(NUM_RX) " TX=" STR(NUM_TX); }
     void registerFrameHandler(void (*handler)(uint8_t *frame, unsigned len)) {
          framehandler = handler;
     };
};

#endif  /* TSP_H */
