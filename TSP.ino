#include "TSP.h"
#include <Wire.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Defines
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define I2C_ADDR (uint8_t)0x25

#define REP_AdcDbg 0x60
#define REP_VER 0xFB
#define REP_ACK 0xF0
#define REP_PARAM_READ 0xCF
#define REP_TRACE 0x90
#define REP_GESTIC 0xFD

#define CMD_VER        0xFF
#define CMD_ECHO       0x04
#define CMD_READ_FLASH 0x17
#define CMD_SETPARAM   0xE0
#define CMD_GETPARAM   0xE1

#define I2C_MAP_TOUCHSTATUS 0x00
#define I2C_MAP_RXRDY 0xFB
#define I2C_MAP_RXBUF 0xFC
#define I2C_MAP_TXRDY 0xFD
#define I2C_MAP_TXBUF 0xFE

#define I2C_ADDR_WR 0x4A
#define I2C_ADDR_RD 0x4B

#define CHECK(x) if (uint8_t res = x) { Serial.printf("Error calling `%s': %d\n", #x, res); }


// const char* SPACE = " ";

const uint8_t numRX = 27;
// Indices are RX lines for each pin
// const uint8_t rxMap[numRX] = { 21, 22, 23, 24, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 25, 26, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
// indices are pin numbers of each RX line
const uint8_t rxMap[numRX] = { 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 0, 1, 2, 3, 15, 16 };


bool TSP::init() {
     pinMode(A6, OUTPUT);
     digitalWrite(A6,LOW);
     delay(1);
     digitalWrite(A6,HIGH);
     
     Wire.begin();
     Wire.setClock(100000);
     Wire.setTimeout(1000);

     Serial.println("\nInitialising");
     delay(10);

     msgLen = 0;

     for (uint8_t i=0; i!=numRX; ++i) {
          delay(100);
          setParameter(0x0200+i, rxMap[i], 0xff);
          // waitForAck(CMD_SETPARAM);
          delay(1000);
          printParameter(0x0200+i);
     }
     return true;
}

void TSP::getRegisters(byte reg, byte size, byte *buffer) {
     Wire.beginTransmission(I2C_ADDR);
     Wire.write(reg);
     if (Wire.endTransmission(false) != 0)
          Serial.println("[TSP getRegisters] I²C transmission failed");
     Wire.requestFrom(I2C_ADDR, size);

     byte i = 0;
     while (Wire.available()) {
          buffer[i++] = Wire.read();
     }

     if (Wire.endTransmission(true) != 0)
          Serial.println("[TSP getRegisters] I²C transmission end 2 failed");  
}

uint8_t TSP::printTouches() {
     getRegisters(I2C_MAP_TOUCHSTATUS, 0x38,touchBuffer);

     uint8_t touchStatus = touchBuffer[0], num;
     num = touchStatus & 0x0F;

     if (num == 0) 
          return touchStatus;     
     
     Serial.printf("Getting %d touches (status=%02X\n", num,touchStatus);
     for (uint8_t i = 0; i<num; ++i) {
          uint8_t
               offset = i*6,
               irts   = *(uint8_t *)(touchBuffer+offset),
               id     = *(uint8_t *)(touchBuffer+offset+1);
          uint16_t
               x      = *(uint16_t *)(touchBuffer + offset + 2), 
               y      = *(uint16_t *)(touchBuffer + offset + 4);
          
          Serial.printf("   Touch [%02X] %d at (%3d,%3d)\n", irts, id, x/328, y/328); // Scaled touch location to 0-100
     }
     return touchStatus;
}


void TSP::readInfo() {
     uint8_t touchStatus = printTouches();
     if (touchStatus & 0x10) // Stream ready
          interpretReply();
}

void TSP::sendCommand(uint8_t cmd, const uint8_t *data, uint8_t len) {
     uint8_t available = 0;
     while (available < len+2) {
          getRegisters(I2C_MAP_RXRDY,1, &available);
          // Serial.printf("sendCommand: %d bytes available\n", available);
          delay(1);
     }     
     Wire.beginTransmission(I2C_ADDR);
     Wire.write(I2C_MAP_RXBUF); // message
     Wire.write(len+1);
     Wire.write(cmd);
     for (uint8_t i=0; i!= len; ++i,++data) {
          Wire.write(*data);
     }
     CHECK(Wire.endTransmission());
}

void TSP::gobble(uint8_t cmd) {
     gobbleTillCmdAck = cmd;
     gobbled = 0;
     Serial.printf("Gobbling command %02X\n",cmd);
}

void TSP::processByte (uint8_t b) {
     Serial.printf(" %02X",b);
     if (gobbleTillCmdAck) {
          if (gobbled == 0 && b == 0x02) {
               gobbled = 1;
//        Serial.println("Gobbled 0x02");
               return;
          }
          if (gobbled == 1) {
               if (b == REP_ACK) {
                    gobbled = 2;
//          Serial.println("Gobbled REP_ACK");
                    return;
               } else {
                    gobbled = 0;
//          Serial.println("No ACK, resetting");
                    return;
               }
          }
          if (gobbled == 2 && b == gobbleTillCmdAck) {
               // Successfully gobbled
               gobbled = 0;
               gobbleTillCmdAck = 0;
//        Serial.printf("Gobbled acknowledgement 0x%02x, msgLen=%d\n", b, msgLen);
               return;
          } else {
               gobbled = 0;
//        Serial.println("No command, resetting");
               return;
          }
     }
     if (msgLen==0) {
          msgLen = b; // b bytes remaining in message
          locationInMsg = 0;
          return;
     } 
     if (locationInMsg == 0) { // First byte of message is command
          currentCmd = b;
          locationInMsg++;
          return;
     }
     buffer[locationInMsg-1] = b;
     locationInMsg++;
     if (locationInMsg == msgLen) {
          switch (currentCmd) {
          case REP_VER: // Get version
               Serial.printf("\n                    Firmware release r%d\n",*(uint16_t *)(buffer+1));
               gobble(CMD_VER);
               break;
          case REP_PARAM_READ:
               Serial.printf("\n                    Read parameter @%04x: [", *(uint16_t*)(buffer));
               for (byte i=2; i<msgLen-1; ++i) // msglen contains command, buffer starts at addr.
                    Serial.printf("%02X", buffer[i]);
               Serial.print("](");
               for (byte i=2; i<msgLen-1; ++i) // msglen contains command, buffer starts at addr.
                    Serial.printf("%d,", buffer[i]);
               Serial.println(")");
               break;
          case REP_TRACE:
               Serial.printf("\n                    Trace: [");
               for (byte i=0; i<msgLen-1; ++i) // msglen contains command
                    Serial.printf("%02X", buffer[i]);
               Serial.println(']');
               break;
          case REP_ACK:
               if (msgLen != 2) {
                    Serial.printf("This ain't no ack\n");
                    break;
               }
               Serial.printf("\n                    Got acknowledgement for cmd %02X\n", buffer[0]);
               lastAck = buffer[0];
               break;

          default:
               Serial.printf("\n                    Ignoring %02x, data [", currentCmd);
               for (byte i=0; i<msgLen-1; ++i) // msglen contains command
                    Serial.printf("%02X",buffer[i]);
               Serial.println(']');
               break;
          }
          msgLen = 0;
          locationInMsg = 0;
//      Serial.printf("resetting message parameters %d %d\n", msgLen, locationInMsg);
     }
}


#define INC 64
void TSP::interpretReply() { 
//     delayMicroseconds(100);

     Wire.beginTransmission(I2C_ADDR);
     Wire.write(I2C_MAP_TXRDY);
     CHECK(Wire.endTransmission(false));
     Wire.requestFrom(I2C_ADDR,(uint8_t)1);
     byte n = Wire.read();
     if (n == 0 || n == 255)
          return;
     Serial.printf("interpretReply(1) n=%d\n",n);
     while (n != 0) {
          if (n>INC)
               n = INC;
      
          Wire.beginTransmission(I2C_ADDR);
          Wire.write(I2C_MAP_TXBUF);
          CHECK(Wire.endTransmission(false));
          n = Wire.requestFrom(I2C_ADDR,n);
          Serial.printf("   -> actually got %d\n", n);
          while (n != 0) {
               byte x = Wire.read();
               processByte(x);
               --n;
               // if (r==0 && Wire.available()) {
               //      Serial.println("r equals 0 but there is still data in the queue");
               //      break;
               // }
          }

          Wire.beginTransmission(I2C_ADDR);
          Wire.write(I2C_MAP_TXRDY);
          CHECK(Wire.endTransmission(false));
          Wire.requestFrom(I2C_ADDR,(uint8_t)1);
          n = Wire.read();
          if (n == 0 || n == 255)
               break;
          Serial.printf("interpretReply(2) n=%d\n",n);
     }
};

void TSP::waitForAck(uint8_t cmd) { 
     bool ack = false;
     while (! ack) {     
          Wire.beginTransmission(I2C_ADDR);
          Wire.write(I2C_MAP_TXRDY);
          CHECK(Wire.endTransmission(false));
          Wire.requestFrom(I2C_ADDR,(uint8_t)1);
          byte n = Wire.read();
          if (n == 0 || n == 255) {
               delay(1);
               continue;
          }
          Serial.printf("waitForAck(1) n=%d\n",n);

          while (true) {
               if (n>INC)
                    n = INC;

               Wire.beginTransmission(I2C_ADDR); 
               Wire.write(I2C_MAP_TXBUF);
               CHECK(Wire.endTransmission(false));
               Wire.requestFrom(I2C_ADDR,n);
               while (Wire.available()) {
                    byte x = Wire.read();
                    processByte(x);
                    ack |= (lastAck == cmd);
                    

                    // if (r==0 && Wire.available()) {
                    //      Serial.println("r equals 0 but there is still data in the queue");
                    //      break;
                    // }
               }
               if (ack)
                    break;

               // Read the current value of TXRDY into n
               Wire.beginTransmission(I2C_ADDR);
               Wire.write(I2C_MAP_TXRDY);
               CHECK(Wire.endTransmission(false));
               Wire.requestFrom(I2C_ADDR,(uint8_t)1);
               n = Wire.read();
               if (n == 0 || n == 255) {
                    break;
               }
               Serial.printf("waitForAck(2) n=%d\n",n);
          }
     }
};


void TSP::printVersion() {
     Serial.println("Firmware Version");
     sendCommand(CMD_VER,NULL,0);
     delayMicroseconds(100);
     interpretReply();
}

void TSP::printParameter(uint16_t addr) {
     uint8_t *c = (uint8_t *)&addr;
     Serial.printf("Getting parameter %04X\n", *(uint16_t *)c);
     sendCommand(CMD_GETPARAM, c, 2);
     delayMicroseconds(100);
     interpretReply();
}

void TSP::setParameter(uint16_t addr, uint32_t data, uint32_t mask) {
     uint8_t buffer[10];
     Serial.printf("setParameter @%04X [%08X] mask [%08X]\n", addr, data, mask);

     *(uint16_t *)buffer     = addr;
     *(uint32_t *)(buffer+2) = data;
     *(uint32_t *)(buffer+6) = mask;
     sendCommand(CMD_SETPARAM, buffer, 10);
     delayMicroseconds(100);
     interpretReply();    
}
