#include "TSP.h"
// #include <Wire.h>
#include "I2C.h"

#define I2C_ADDR_WR 0x4A
#define I2C_ADDR_RD 0x4B

bool TSP::init() {
     pinMode(SDA, INPUT_PULLUP);
     pinMode(SCL, INPUT_PULLUP);

     digitalWrite(SDA,LOW);
     digitalWrite(SCL,LOW);  

     // Wire.begin();
     // Wire.setClock(400000);
     // Wire.setTimeout(1000);
     I2c.begin();
     I2c.setSpeed(true);
     I2c.pullup(true);
     I2c.timeOut(1000);
     Serial.println("Initialising");
     delay(10);

     msgLen = 0;

     return true;
}

void TSP::getRegisters(byte reg, byte size, byte *buffer) {
     // Wire.beginTransmission(I2C_ADDR);
     // Wire.write(reg);
     // if (Wire.endTransmission(false) != 0)
     //      Serial.println("[TSP getRegisters] I²C transmission failed");
     // Wire.requestFrom(I2C_ADDR, size);

     // byte i = 0;
     // while (Wire.available()) {
     //      buffer[i++] = Wire.read();
     // }

     // if (Wire.endTransmission(true) != 0)
     //      Serial.println("[TSP getRegisters] I²C transmission end 2 failed");  
}

uint8_t TSP::printTouches() {
     I2c._start();
     I2c._sendAddress(I2C_ADDR_WR);
     I2c._sendByte(0x00);       // register address 0x00
     I2c._start();
     I2c._sendAddress(I2C_ADDR_RD);
     uint8_t touchStatus, num, res;
     res = I2c._receiveByte(true, &touchStatus);
     if (res != 0)
          Serial.printf("I2C receiveByte returned %d\n", res);
     num = touchStatus & 0xff;
     if (num == 0) {
          res = I2c._receiveByte(false, &num);
          I2c._stop();
          return touchStatus;
     }
     
     Serial.printf("Getting %d touches\n", num);
     uint8_t numBytes = num * 6;
     for (uint8_t i = 0; i+1<numBytes; ++i) {
          res = I2c._receiveByte(true, touchBuffer+i);
          if (res != 0)
               Serial.printf("I2C receiveByte[%d] returned %d\n", i, res);
     }
     res = I2c._receiveByte(false, touchBuffer+numBytes-1);
     I2c._stop();

     for (uint8_t i = 0; i<num; ++i) {
          uint8_t
               offset = i*6,
               irts   = *(uint8_t *)(touchBuffer+offset),
               id     = *(uint8_t *)(touchBuffer+offset+1);
          uint16_t
               x = *(uint16_t *)(touchBuffer + 2), 
               y = *(uint16_t *)(touchBuffer + 4);
          Serial.printf("   Touch [%02X] %d at (%d,%d)\n", touchBuffer[0], id, x, y);
     }
}


void TSP::readInfo() {
     uint8_t touchStatus = printTouches();
     if (touchStatus & 0x10) // Stream ready
          interpretReply();
}

void TSP::sendCommand(uint8_t cmd, const uint8_t *data, uint8_t len) {
//      Wire.beginTransmission(I2C_ADDR);
//      Wire.write(0xFC); // message
//      Wire.write(len+1);
//      Wire.write(cmd);
//      for (uint8_t i=0; i!= len; ++i,++data) {
//           Wire.write(*data);
//      }
//      Wire.endTransmission();
}

void TSP::gobble(uint8_t cmd) {
     gobbleTillCmdAck = cmd;
     gobbled = 0;
     Serial.printf("Gobbling command %02X\n",cmd);
}

void TSP::processByte (uint8_t b) {
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
               Serial.printf("\nFirmware release r%d\n",*(uint16_t *)(buffer+1));
               gobble(CMD_VER);
               break;
          case REP_PARAM_READ:
               Serial.printf("Read parameter @%04x: ", *(uint16_t*)(buffer));
               for (byte i=2; i<msgLen-1; ++i) // msglen contains command, buffer starts at addr.
                    Serial.printf("%02X", buffer[i]);
               Serial.println();
               break;
          case REP_TRACE:
               Serial.printf("Trace: [");
               for (byte i=0; i<msgLen-1; ++i) // msglen contains command
                    Serial.printf("%02X", buffer[i]);
               Serial.println(']');
               break;
          case REP_ACK:
               break;

          default:
               Serial.printf("Ignoring %02x, data [", currentCmd);
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


void TSP::interpretReply() { 
//      delayMicroseconds(100);
// #define INC 16
//      Wire.beginTransmission(I2C_ADDR);
//      Wire.write(0xFD);
//      Wire.endTransmission(false);
//      Wire.requestFrom(I2C_ADDR,(uint8_t)1);
//      byte n = Wire.read();
//      if (n == 0)
//           return;
//      Serial.printf(" n=%d\n",n);
//      while (n != 0) {
//           byte r = n; 
//           if (r>INC)
//                r = INC;
//           n -= r;   
      
//           Wire.beginTransmission(I2C_ADDR);
//           Wire.write(0xFE);
//           Wire.endTransmission(false);
// //      delayMicroseconds(100);
//           Wire.requestFrom(I2C_ADDR,r);
//           while (Wire.available()) {
//                byte x = Wire.read();
//                Serial.printf("%02X",x);
//                processByte(x);
//                Serial.print(SPACE);
//                r--;
//                if (r==0 && Wire.available()) {
//                     Serial.println("r equals 0 but there is still data in the queue");
//                     break;
//                }
//           }
//           if (r != 0) {
//                Serial.print(" Nothing available, not all was read... "); Serial.print(r);
//                delay(1);
//                n += r;  // Put back in the amount left to read.
//           }
//           Serial.println();
//      }
};

void TSP::printVersion() {
     Serial.println("Firmware Version");
     sendCommand(0xff,NULL,0);
     delay(1);
     interpretReply();
}

void TSP::printParameter(uint16_t addr) {
     uint8_t *c = (uint8_t *)&addr;
     Serial.printf("Getting parameter %04X\n", *(uint16_t *)c);
     sendCommand(0xE1, c, 2);
     delay(1);
     interpretReply();
}

void TSP::setParameter(uint16_t addr, uint32_t data, uint32_t mask) {
     uint8_t buffer[10];
     Serial.printf("setParameter @%04X [%08X] mask [%08X]\n", addr, data, mask);

     *(uint16_t *)buffer     = addr;
     *(uint32_t *)(buffer+2) = data;
     *(uint32_t *)(buffer+6) = mask;
     sendCommand(0xE0, buffer, 10);
     interpretReply();    
}
