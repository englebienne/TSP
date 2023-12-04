#include "TSP.h"
#include <Wire.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Defines
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define I2C_ADDR           (uint8_t)0x25

#define REP_ECHO            0x04
#define REP_FLASHCONTENTS   0x17
#define REP_AdcDbg          0x60
#define REP_TRACE           0x90
#define REP_SWIPE           0xA0
#define REP_SCROLL          0xA1
#define REP_TAP             0xA2
#define REP_NOISE           0xB0
#define REP_MUT_NORM_SEC    0xC3
#define REP_ACK             0xF0
#define REP_PARAM_READ      0xCF
#define REP_TOUCH_FILT      0xF2
#define REP_TOUCH_PRED      0xF3
#define REP_TOUCH_RAW       0xF4
#define REP_TOUCH_P16       0xF5
#define REP_SELF_RAW        0xFA
#define REP_SELF_NORM       0xFD
#define REP_VER             0xFB

#define CMD_VER           0xFF
#define CMD_ECHO          0x04
#define CMD_READ_FLASH    0x17
#define CMD_SETPARAM      0xE0
#define CMD_GETPARAM      0xE1
#define CMD_GESTIC        0xFD
#define CMD_FORCEBASELINE 0xFB

#define I2C_MAP_TOUCHSTATUS 0x00
#define I2C_MAP_RXRDY 0xFB
#define I2C_MAP_RXBUF 0xFC
#define I2C_MAP_TXRDY 0xFD
#define I2C_MAP_TXBUF 0xFE

#define I2C_ADDR_WR 0x4A
#define I2C_ADDR_RD 0x4B

#define PAR_NUM_RX_CHAN 0x0100
#define PAR_NUM_TX_CHAN 0x0101
#define PAR_MGC3130     0x0102
#define PAR_NVDM        0x0080
#define PAR_NVAM        0x0081
#define PAR_STREAM      0x0082


#define BITMASK(b) ((uint16_t)1 << b)

#define NVAM_GEST       BITMASK(0)
#define NVAM_FULLSCAN   BITMASK(2)
#define NVAM_FL_EVENT   BITMASK(5)
#define NVAM_SW_EVENT   BITMASK(6)
#define NVAM_AW_EVENT   BITMASK(7)
#define NVAM_BESTFREQ   BITMASK(8)
#define NVAM_AUTOBASE   BITMASK(9)
#define NVAM_DIGITIZER  BITMASK(10)
#define NVAM_DECODE     BITMASK(11)
#define NVAM_FULLMASK   (NVAM_GEST | NVAM_FULLSCAN | NVAM_FL_EVENT | NVAM_SW_EVENT | NVAM_AW_EVENT | NVAM_BESTFREQ | NVAM_AUTOBASE | NVAM_DIGITIZER | NVAM_DECODE)

#define NVDM_SELFNORM   BITMASK(0)
#define NVDM_MUTCACHE   BITMASK(1)
#define NVDM_SELFRAW    BITMASK(2)
#define NVDM_MUTRAW     BITMASK(3)
#define NVDM_ADC        BITMASK(4)
#define NVDM_ADC_COR    BITMASK(5)
#define NVDM_TRACE      BITMASK(8)
#define NVDM_NOISE      BITMASK(9)
#define NVDM_RAWPOS     BITMASK(10)
#define NVDM_FINGERPOS  BITMASK(11)
#define NVDM_GESTURE    BITMASK(12)
#define NVDM_CUSTOM     BITMASK(13)
#define NVDM_DIAG       BITMASK(14)
#define NVDM_GESTIC     BITMASK(15)


#define CHECK(x) if (uint8_t res = x) { Serial.printf("%s - ERROR calling `%s': %d\n", __FUNCTION__, #x, res); }
#define CHECK_RETURN(x) if (uint8_t res = x) { Serial.printf("%s - ERROR calling `%s': %d\n", __FUNCTION__, #x, res); delayMicroseconds(1); gobble(0x01); return; }
#define INDENT(...) Serial.printf("\n                              " __VA_ARGS__)


// const char* SPACE = " ";

const uint8_t numRX = 27;

// Not sure which of the following is right:
// Indices are RX lines for each pin (seems most logical, and it turns out that the default values for the TX pins are, in this format, correspoding to the layout of the board):
const uint8_t rxMap[numRX] = { 21, 22, 23, 24, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 25, 26, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
// or
// indices are pin numbers of each RX line:
// const uint8_t rxMap[numRX] = { 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 0, 1, 2, 3, 15, 16 };


void TSP::handleInterrupt() {
     // Ideally, this should initiate the I2C interaction to read from the device, but
     // the Wire library does not support that. So, we set a flag that we poll to check if something's available
     //
     // However. this does not seem to work (falls, doesn't raise? Not sure)
     streamAvailable = true;
}

#define MAXREAD 128
void TSP::getData() {     
     // if (digitalRead(IRQ_PIN) != LOW) { // Instead of checking `streamAvailable`, directly check pin.          
     //      // Seems to be mostly low, even when no data in buffer. But no data seems to be in buffer when high
     //      // Checking the STR bit in TOUCHSTATUS makes things a lot less stable          
     //      Serial.print(".");
     //      return;
     // }
     // Serial.print("SA");

     for (uint8_t i=0; i<4; ++i) {             // Repeatedly read from the buffer to be sure we get everything we need
          Wire.beginTransmission(I2C_ADDR);
          Wire.write(I2C_MAP_TXRDY);
          if (byte res = Wire.endTransmission(false)) {
               Serial.printf("%s: Wire.endTransmission(false) returned %d\n", __PRETTY_FUNCTION__, res);
               delayMicroseconds(100);
               return;
          }
          Wire.requestFrom(I2C_ADDR,(uint8_t)1);
          byte n = Wire.read();
          if (byte res = Wire.endTransmission()) {
               Serial.printf("%s: Wire.endTransmission() returned %d\n", __PRETTY_FUNCTION__, res);
               delayMicroseconds(100);
          }
          
          if (n == 0 || n==255)  {
               // Serial.print("x");
               streamAvailable = false;     
               return;
          }

          byte touchstatus;
          getRegisters(I2C_MAP_TOUCHSTATUS,1, &touchstatus);
          if (!(touchstatus & 0x10)) {
               Serial.printf("n=%d, but STR not set\n", n);
               return;
          }

          if (n>MAXREAD)
               n = MAXREAD;

          // Serial.printf("getData %d bytes... ",n);
          // while (Wire.available()) {
          //      Serial.printf("WTF YO ");
          //      circBuffer[wrIdx] = Wire.read();
          //      if ( (wrIdx+1) % buflen == rdIdx)
          //           Serial.printf("ERROR: WRITE POINTER CAUGHT UP WITH READ POINTER!\n");
          //      incIdx(wrIdx);
          // }
               
          Wire.beginTransmission(I2C_ADDR);
          Wire.write(I2C_MAP_TXBUF);
          if (byte res = Wire.endTransmission(false)) {
               Serial.printf("%s: Wire.endTransmission(false) - 2 - returned %d\n", __PRETTY_FUNCTION__, res);
               delayMicroseconds(10);
               return;
          }
          n = Wire.requestFrom(I2C_ADDR,(uint8_t)n);
          if (byte res = Wire.endTransmission()) {
               // Wire.clearTimeoutFlag();
               Serial.printf("%s: Wire.endTransmission() - 2 - returned %d after getting %d bytes\n", __PRETTY_FUNCTION__, res, n);
               delayMicroseconds(10);
          }
          Serial.printf("got %d\n",n);

          while (Wire.available()) {
               --n;
               // circBuffer[wrIdx] = Wire.read();
               // Serial.printf("%02X@%d ",circBuffer[wrIdx], wrIdx);
               // if ( (wrIdx+1) % buflen == rdIdx)
               //      Serial.printf("ERROR: WRITE POINTER CAUGHT UP WITH READ POINTER!\n");
               // incIdx(wrIdx);
               processByte(Wire.read());
          }
          // delayMicroseconds(10); // Not sure this is needed, but being too fast makes things less stable... Being too slow too?
     }
}     

/**
 * \brief Wait for the chip to react, and poll the bus for messages in the meantime
 * \param msec How long to wait for
 * 
 * 2023/11/30: GWENN - First version
 * 
 **/
void TSP::delayAndPoll(uint16_t msec) {
     for (uint16_t i=0; i!=msec; ++i) {
          interpretReply();
          delayMicroseconds(10);
     }
}

/**
 * \brief Initialise the chip
 * \return success
 * 
 * 2023/11/30: GWENN - First version
 * 
 **/
bool TSP::init() {
     pinMode(TSP_RESET_PIN, OUTPUT);       // Connected to reset
     // Allows us to reset communication when we start up (again)

     // Setup I2C connection
     Wire.begin();
     Wire.setClock(400000);
     Wire.setTimeout(10000);

     
     Serial.println("\nInitialising");
     digitalWrite(TSP_RESET_PIN,LOW);      // Reset the MTCH6303
     delay(1000);
     digitalWrite(TSP_RESET_PIN,HIGH);
     delay(1000);                // Delay of 1 resulted in NACKs -> Reset works :-)

     printParameter(PAR_NVAM);  // Get the list of currently active modules 
     
     setParameter(PAR_NVAM, 0x0, NVAM_FULLMASK); // Disable all modules while parameters are set

     delayAndPoll(10);
     printParameter(PAR_NVAM);  // debug: Verify everything's off (it is)

     for (uint8_t i=0; i!=numRX; ++i) { // Set the RX - OUT pin map
          delayAndPoll(1);
          setParameter(0x0200+i, rxMap[i], 0xff);
     }
     for (uint8_t i=0; i!=numRX; ++i) { // debug Verify the RX - OUT pin map is well set
          delayAndPoll(1);
          printParameter(0x0200+i);
     }

     // setParameter(PAR_NVAM,     // Activate relevant modules (I think. It's unclear from the dataset what they do, exactly
     //              NVAM_DECODE | NVAM_DIGITIZER | NVAM_AUTOBASE | NVAM_BESTFREQ | NVAM_FULLSCAN,
     //              NVAM_FULLMASK);
     setParameter(PAR_NVAM,     // Activate relevant modules (I think. It's unclear from the dataset what they do, exactly
                  NVAM_DIGITIZER|NVAM_AUTOBASE|NVAM_BESTFREQ,
                  NVAM_FULLMASK);
     delayAndPoll(10);
     printParameter(PAR_NVAM);  // Verify the modules are activated
     delayAndPoll(10);
     sendCommand(CMD_FORCEBASELINE,NULL,0); // Get a baseline noise measurement
     delayAndPoll(20);

#define DBG NVDM_SELFNORM
     setParameter(PAR_NVDM,     // Activate debug modules. I want to figure out how to read raw capacitance measurements
                  DBG,
                  DBG);
     
     printParameter(PAR_NVDM);  // debug: Check what's active
     return true;
}

/**
 * \brief Basic function to read data from the I2C map 
 * \param reg The I2C register address (Table 3-5)
 * \param size How many bytes to read
 * \param buffer Where to store them
 * \return Number of bytes read
 * 
 * 2023/11/30: GWENN - First version
 * 
 **/
byte TSP::getRegisters(byte reg, byte size, byte *buffer) {
     Wire.beginTransmission(I2C_ADDR);
     Wire.write(reg);
     if (uint8_t res = Wire.endTransmission(false))
          Serial.printf("[TSP getRegisters] endTransmission failed: %d\n", res);

     Wire.requestFrom(I2C_ADDR, size);
     byte i = 0;
     while (Wire.available()) 
          buffer[i++] = Wire.read();

     // Is this necessary?
     if (uint8_t res = Wire.endTransmission(true))
          Serial.printf("[TSP getRegisters] endTransmission failed: %d\n", res);
     return i;
}

/**
 * \brief Print the reported  touches (1-10) to serial port (Table 3-5 0x01 - 0x37)
 * \return TouchStatus
 * 
 * 2023/11/30: GWENN - First version
 * 
 **/
uint8_t TSP::printTouches() {
     getRegisters(I2C_MAP_TOUCHSTATUS, 0x38,touchBuffer); // There's no need to read beyond TOUCHSTATUS & 0x0f, but
     // the Wire library does not allow to modify the number of bytes to read while reading (and we should not stop-
     // start between reading TOUCHSTATUS and the actual data, because they then can become inconsistent)

     uint8_t touchStatus = touchBuffer[0], num;
     num = touchStatus & 0x0F;

     if (num == 0) 
          return touchStatus;

     uint8_t *tb = touchBuffer+1; // Skip touchstatus byte
     
     Serial.printf("Getting %d touches (status=%02X):\n", num,touchStatus);
     Serial.print("          FLAGS: ");
     if (touchStatus & 0x10) Serial.print("STR ");
     if (touchStatus & 0x20) Serial.print("GST ");
     if (touchStatus & 0x40) Serial.print("MGC ");
     if (touchStatus & 0x80) Serial.print("R ");
     Serial.println();
     
     for (uint8_t i = 0; i<num; ++i) {
          uint8_t
               offset = i*6,
               irts   = *(uint8_t *)(tb+offset),
               id     = *(uint8_t *)(tb+offset+1);
          uint16_t
               x      = *(uint16_t *)(tb + offset + 2), 
               y      = *(uint16_t *)(tb + offset + 4);
          
          Serial.printf("   Touch [%02X] %d at (%3d,%3d)\n", irts, id, x/328, y/328); // Scaled touch location to 0-100
     }
     return touchStatus;
}


/**
 * \brief Poll touch status, touch, and stream messages
 * 
 * 2023/11/30: GWENN - First version
 * 
 **/
void TSP::readInfo() {
     // uint8_t touchStatus = printTouches();
     // if (touchStatus & 0x10) // Stream ready
          interpretReply();
}

/**
 * \brief Send a command to the chip
 * \param cmd  The command ID
 * \param data The data associated
 * \param len  The length of the data
 * 
 * 2023/11/30: GWENN - First version
 * 
 **/
void TSP::sendCommand(uint8_t cmd, const uint8_t *data, uint8_t len) {
     // Verify that the chip can take the message
     uint8_t available = 0;
     while (available < len+2) {
          getRegisters(I2C_MAP_RXRDY,1, &available);
          // Serial.printf("sendCommand: %d bytes available\n", available);
//          delay(1);
     }
     
     Wire.beginTransmission(I2C_ADDR);
     Wire.write(I2C_MAP_RXBUF); // message
     Wire.write(len+1);
     Wire.write(cmd);
     for (uint8_t i=0; i!= len; ++i,++data) {
          Wire.write(*data);
     }
     CHECK(Wire.endTransmission());
     delayAndPoll(1);
     interpretReply();          // Make sure we keep up with stream back...
}

/**
 * \brief Ignore all bytes until we reach an acknowledgement of command CMD
 * \param cmd The command to wait for (0x01 for any command)
 * 
 * This affects how "processByte", below, behaves
 * 
 * 
 * 2023/11/30: GWENN - First version
 * 
 **/
void TSP::gobble(uint8_t cmd) {
     gobbleTillCmdAck = cmd;
     gobbled = 0;
     Serial.printf("Gobbling until %02X\n",cmd);
     msgLen = 0;
     byte v = 0xDE;
     sendCommand(CMD_ECHO,&v,1); // Ensure an ack is comming.
}

/**
 * \brief Printout the parameter value in human-readable format
 * \param buffer 2 bytes of parameter address, rest is data
 * 
 * length of message is global to the object
 * 
 * 2023/11/30: GWENN - First version
 * 
 **/
void TSP::interpretParam(uint8_t *buffer) {

     INDENT("Read parameter @%04x: [", *(uint16_t*)(buffer));
     for (byte i=2; i<msgLen-1; ++i) // msglen contains command, buffer starts at addr.
          Serial.printf("%02X", buffer[i]);
     Serial.print("](");
     for (byte i=2; i<msgLen-1; ++i) // msglen contains command, buffer starts at addr.
          Serial.printf("%d,", buffer[i]);
     Serial.println(")");

     uint16_t addr = *(uint16_t *)buffer;
     uint16_t payload8 = *(uint8_t *)(buffer+2);
     uint16_t payload16 = *(uint16_t *)(buffer+2);
     uint16_t payload32 = *(uint32_t *)(buffer+2);
     
     switch (addr) {
     case PAR_NVAM:
          INDENT("Flags of NVAM register: [");

          if (payload16 & NVAM_GEST)      Serial.print("GEST ");
          if (payload16 & NVAM_FULLSCAN)  Serial.print("FULLSCAN ");
          if (payload16 & NVAM_FL_EVENT)  Serial.print("FLICK ");
          if (payload16 & NVAM_SW_EVENT)  Serial.print("SWIPE ");
          if (payload16 & NVAM_AW_EVENT)  Serial.print("AIRWHEEL ");
          if (payload16 & NVAM_BESTFREQ)  Serial.print("BESTFREQ ");
          if (payload16 & NVAM_AUTOBASE)  Serial.print("AUTOBASE ");
          if (payload16 & NVAM_DIGITIZER) Serial.print("DIGITIZER ");
          if (payload16 & NVAM_DECODE)    Serial.print("DECODE ");
          Serial.println("]");
          break;
          
     case PAR_NVDM:
          INDENT("Flags of NVDM register: [");

#define BITP(b) if (payload16 & NVDM_##b) Serial.print(#b " ")
          BITP(SELFNORM);
          BITP(MUTCACHE);
          BITP(SELFRAW);
          BITP(MUTRAW);
          BITP(ADC);
          BITP(ADC_COR);
          BITP(TRACE);
          BITP(NOISE);
          BITP(RAWPOS);
          BITP(FINGERPOS);
          BITP(GESTURE);
          BITP(CUSTOM);
          BITP(DIAG);
          BITP(GESTIC);
          Serial.println("]");
          break;
     }
}

void printBuffer(uint8_t *buf, uint8_t len) {
     Serial.print("[");
     for (byte i=0; i<len; ++i) 
          Serial.printf("%02X", buf[i]);
     Serial.println(']');
}


/**
 * \brief Process the next byte
 * \param b 
 * 
 * 2023/11/30: GWENN - First version
 * 
 **/
void TSP::processByte (uint8_t b) {
     Serial.printf(" [%02X]-%d",b,locationInMsg); // Debug     
     
     if (gobbleTillCmdAck) {    // Ignore bytes to resynch the stream, until we reach [0x02 0xF0 <cmd>]
          Serial.print("-G"); //"
          if (gobbled == 0 && b == 0x02) {
               gobbled = 1;                         // Serial.println("Gobbled 0x02");
               return;
          }
          if (gobbled == 1) {
               if (b == REP_ACK) {
                    gobbled = 2;                    // Serial.println("Gobbled REP_ACK");
                    return;
               } else {
                    gobbled = 0;                    // Serial.println("No ACK, resetting");
                    return;
               }
          }
          if (gobbled == 2 && (b == gobbleTillCmdAck || b == CMD_ECHO || gobbleTillCmdAck == 0x01)) {
                                // Successfully gobbled
               gobbled = 0;
               gobbleTillCmdAck = 0;
                                // Serial.printf("Gobbled acknowledgement 0x%02x, msgLen=%d\n", b, msgLen);
               msgLen = 0;      // Start a new message
               return;
          } else {
               gobbled = 0;                         // Serial.println("No command, resetting");
               return;
          }
     }
     
     if (msgLen==0) {                               // Starting a new message, first byte is length
          msgLen = b;                               // b bytes remaining in message

          //////////////////////////////////////////////////
          //////////// Ugly stability hacks ////////////////
          //////////////////////////////////////////////////

          switch (msgLen) {
          case REP_PARAM_READ:                      // We lost the length parameter to a REP_PARAM_READ msg
          case REP_TOUCH_FILT:
          case REP_TOUCH_PRED:
          case REP_TOUCH_RAW:
          case REP_TOUCH_P16:
          case REP_SELF_RAW:
          case REP_SELF_NORM:
          case CMD_GETPARAM:
          case CMD_SETPARAM:
          case REP_VER:
               INDENT("ERROR: Msg Len is unlikely (%02X). Gobbling until 01\n", msgLen);
               gobble(0x01);
               return;
          case REP_ACK:                             // ACK len got lost
               INDENT("ERROR: Msg Len is unlikely (%02x) Assuming length of REP_ACK missing\n", msgLen);
               msgLen = 0x02;
               currentCmd = REP_ACK;
               locationInMsg = 1;
               return;
          case 0:
          case 1:               // Definitely invalid messages
               gobble(0x01);
               return;
               
          }
          /////////////////// End of hacks ////////////////////////////

          locationInMsg = 0;
          return;
     } 
     if (locationInMsg == 0) {                        // First byte of message is command
          currentCmd = b;
          if (currentCmd == REP_VER)
               msgLen = 0x10;
          locationInMsg++;
          return;
     }

     // We're getting data now.
     // Serial.printf("%s: locationInMsg=%d\n",__FUNCTION__, locationInMsg);
     msgBuffer[locationInMsg-1] = b;
     locationInMsg++;
     if (locationInMsg != msgLen)
          return;
          
     // End of message reached
     switch (currentCmd) {
     case REP_ECHO:
          INDENT("Echo? What you playin' at? ");
          printBuffer(msgBuffer, msgLen-1);
          break;
     case REP_FLASHCONTENTS:
          INDENT("Flash contents: ");
          printBuffer(msgBuffer,msgLen-1);
          break;
     case REP_AdcDbg:
          INDENT("Adc Dbg: rx=%d, tx=%d, freq=%d", msgBuffer[0], msgBuffer[1], msgBuffer[2]);
          for (uint8_t i=4; i<msgLen; i+=2) {
               if (i%16==0) {
                    Serial.println();
                    INDENT("    ");
               }
               Serial.printf(" %04X", *(uint16_t *)(msgBuffer+i));
          }
          break;
     case REP_TRACE:
          INDENT("Trace: loc=%02X, event=%02X\n", msgBuffer[0], msgBuffer[1]);
          break;
     case REP_SWIPE:
          INDENT("Swipe: flags=%02X, fingers=%d", msgBuffer[0], msgBuffer[1]);
          break;
     case REP_SCROLL:
          INDENT("Scroll: fingers=%d, diamHI=%d, diameter=%d, centre=(%d,%d)\n",
                 msgBuffer[0], msgBuffer[1],
                 *(uint16_t *)(msgBuffer+2), *(uint16_t *)(msgBuffer+4), *(uint16_t *)(msgBuffer+6));
          break;
     case REP_TAP:
          INDENT("Tap: flags=%02X, fingers=%d", msgBuffer[0], msgBuffer[1]);
          break;
     case REP_NOISE:
          INDENT("NOISE: subID=%02x data=");
          printBuffer(msgBuffer+1,msgLen-2);
          break;
     case REP_MUT_NORM_SEC:
          INDENT("Mutual Normalised Section rx=%d, tx=%d, nodes=[");
          for (byte i=2; i < msgLen; i+=2) 
               Serial.printf(" %04x", *(uint16_t *)(msgBuffer+i));
          Serial.println("]");
          break;
     case REP_PARAM_READ:
          interpretParam(msgBuffer);
          break;
     case REP_ACK:
          if (msgLen != 2) {
               Serial.printf(" This ain't no ack\n");
               gobble(0x01);
               break;
          }
          INDENT("Got acknowledgement for cmd %02X\n", msgBuffer[0]);
          lastAck = msgBuffer[0];
          // Serial.printf("rd=%d,wr=%d\n", rdIdx,wrIdx);
          break;
     case REP_TOUCH_FILT:
          INDENT("Touch filtered "); 
          for (byte i=0; i+1<msgLen; i+=5) 
               Serial.printf("[ ID=%02X (%d,%d) ] ", msgBuffer[i], *(uint16_t *)(msgBuffer+i+1), *(uint16_t *)(msgBuffer+i+3));
          Serial.println();
               
          break;
     case REP_TOUCH_PRED:
          INDENT("Touch prediction ");
          for (byte i = 0; i+1 < msgLen; i+=9) {
               Serial.printf("[ ID=%02X (%d,%d)->(%d,%d) ] ",
                             msgBuffer[i], *(uint16_t *)(msgBuffer+i+1), *(uint16_t *)(msgBuffer+i+3),
                             *(uint16_t *)(msgBuffer+i+5), *(uint16_t *)(msgBuffer+i+7));
          }
          Serial.println();
          break;
     case REP_TOUCH_RAW:
          INDENT("Touch RAW ");
          for (byte i=0; i+1<msgLen; i+=5) 
               Serial.printf("[ ID=%02X (%d,%d) ] ", msgBuffer[i], *(uint16_t *)(msgBuffer+i+1), *(uint16_t *)(msgBuffer+i+3));
          Serial.println();
          break;               
     case REP_TOUCH_P16:
          INDENT("Touch Pos16 (pen?) ");
          for (byte i=0; i+1<msgLen; i+=5) 
               Serial.printf("[ ID=%02X (%d,%d) ] ", msgBuffer[i], *(uint16_t *)(msgBuffer+i+1), *(uint16_t *)(msgBuffer+i+3) );
          Serial.println();
          break;              
     case REP_SELF_RAW:
          INDENT("SelfRaw [ ");
          for (byte i=0; i+1<msgLen; i+=2) 
               Serial.printf("%04X ", *(uint16_t *)(msgBuffer+i));
          Serial.println("]");
          break;               
     case REP_SELF_NORM:
          INDENT("SelfNorm [ ");
          for (byte i=0; i+1<msgLen; i+=2) 
               Serial.printf("%04X ", *(uint16_t *)(msgBuffer+i));
          Serial.println("]");
          break; 
     case REP_VER:                             // Get version
          INDENT("Firmware release r%d\n",*(uint16_t *)(msgBuffer+1));
          gobble(CMD_VER);                     // Version message does not respect length parameter
          break;

     default:
          INDENT("Ignoring REPly %02x with data [", currentCmd);
          for (byte i=0; i<msgLen-1; ++i) // msglen contains command
               Serial.printf("%02X",msgBuffer[i]);
          Serial.println(']');
          break;
     }
     msgLen = 0;
     locationInMsg = 0;
//      Serial.printf("resetting message parameters %d %d\n", msgLen, locationInMsg);
}




/**
 * \brief Poll for data and interpret it
 * 
 * 2023/11/30: GWENN - First version
 * 
 **/
void TSP::interpretReply() {
     // Serial.printf("before gd r=%d, w=%d %02x ", rdIdx,wrIdx,circBuffer[rdIdx]);
     // getData();                 // Poll for data
     // Serial.printf("after gd r=%d, w=%d %02x ", rdIdx,wrIdx,circBuffer[rdIdx]);
     
     // while (rdIdx != wrIdx) {   // exit when no data left in queue
     //      Serial.printf("before pr  r=%d, w=%d %02x \n", rdIdx,wrIdx,circBuffer[rdIdx]);
     //      processByte(circBuffer[rdIdx]);
     //      Serial.printf("after  pr  r=%d, w=%d %02x \n", rdIdx,wrIdx,circBuffer[rdIdx]);
     //      incIdx(rdIdx);
     //      Serial.printf("after  inc r=%d, w=%d %02x \n", rdIdx,wrIdx,circBuffer[rdIdx]);
     // }
     getData();
};

/**
 * \brief Get and print the firmware version
 * 
 * 2023/11/30: GWENN - First version
 * 
 **/
void TSP::printVersion() {
     Serial.println("Firmware Version");
     sendCommand(CMD_VER,NULL,0);
     delayAndPoll(10);
     // interpretReply();
}

/**
 * \brief Print the value 
 * \param addr 
 * 
 * 2023/11/30: GWENN - First version
 * 
 **/
void TSP::printParameter(uint16_t addr) {
     uint8_t *c = (uint8_t *)&addr;
     Serial.printf("Getting parameter %04X\n", *(uint16_t *)c);
     sendCommand(CMD_GETPARAM, c, 2);
     delayAndPoll(10);
     // interpretReply();
}

/**
 * \brief Set a parameter value 
 * \param addr The address of the parameter
 * \param data The (bitmask) value to set the paramter to
 * \param mask What bits of this parameter should be affected
 * 
 * 2023/11/30: GWENN - First version
 * 
 **/
void TSP::setParameter(uint16_t addr, uint32_t data, uint32_t mask) {
     uint8_t buffer[10];
     Serial.printf("setParameter @%04X [%08X] mask [%08X]\n", addr, data, mask);

     *(uint16_t *)buffer     = addr;
     *(uint32_t *)(buffer+2) = data;
     *(uint32_t *)(buffer+6) = mask;
     sendCommand(CMD_SETPARAM, buffer, 10);
     delayAndPoll(10);
     // interpretReply();    
}
