#include <Wire.h>
#include "TSP.h"

//#define IRQ 12

byte adc [256];

// void printNibble(char &c, byte b) {
//   if (b<10) 
//     c = '0'+b;
//   else
//     c = 'A'+b-10;
// } 

// void printByte(byte b) {
//   char buffer[3];
//   buffer[2] = 0;
//   printNibble(buffer[0], b>>4);
//   printNibble(buffer[1], b&0x0F);
//   Serial.print(buffer);
// }

// void printBytes(byte *buffer, byte size) {
//   for (byte i=0; i<size; ++i) {
//     Serial.print(SPACE);
//     printByte(buffer[i]);
//   }
// }

// const char *parseByte(const char *s, uint8_t &b) {
//   b=(uint8_t)0;
//   for (int i=0; i<2; ++i) {
//     b <<= 4;    
//     switch (*s) {
//       case 'a':    case 'b':    case 'c':    case 'd':    case 'e':    case 'f':
//         b += *s - 'a';
//         break;
//       case 'A':    case 'B':    case 'C':    case 'D':    case 'E':    case 'F':
//         b += *s - 'A';
//         break;
//       case '0':      case '1':      case '2':      case '3':      case '4':      case '5':      case '6':
//       case '7':      case '8':      case '9':
//         b += *s - '0';
//         break;
//       default:
//         Serial.print("ERROR: could not parse hex byte at ");
//         Serial.print(s);
//         break;
//     }
//     ++s;
//   }
//   return s;
// }

// const char *parseWord(const char *s, uint16_t &b) {
//   b=(uint16_t)0;
//   for (int i=0; i<4; ++i) {
//     b <<= 4;    
//     switch (*s) {
//       case 'a':    case 'b':    case 'c':    case 'd':    case 'e':    case 'f':
//         b += *s - 'a';
//         break;
//       case 'A':    case 'B':    case 'C':    case 'D':    case 'E':    case 'F':
//         b += *s - 'A';
//         break;
//       case '0':      case '1':      case '2':      case '3':      case '4':      case '5':      case '6':
//       case '7':      case '8':      case '9':
//         b += *s - '0';
//         break;
//       default:
//         Serial.print("ERROR: could not parse hex byte at ");
//         Serial.print(s);
//         break;
//     }
//     ++s;
//   }
//   return s;
// }

TSP tsp;

void setup() {
  Serial.begin(921600, SERIAL_8N1);
  while (!Serial) 
    delay(1); // wait for serial port to connect. Needed for native USB
  
  tsp.init();
  tsp.printVersion();
  tsp.printParameter(0x0100);
  tsp.printParameter(0x0101);
  tsp.printParameter(0x0080);
  tsp.printParameter(0x0082);
//  tsp.setParameter((uint16_t)0x0080, 0x00000000, 0x00000ffff);
//  tsp.printParameter((uint16_t)0x0080);

  tsp.printParameter((uint16_t)0x0081);
//  tsp.setParameter((uint16_t)0x0081, 0x0000ffff, 0x0000ffff);
//  tsp.printParameter((uint16_t)0x0081);

//  tsp.interpretReply();  
}



void loop() {
  // put your main code here, to run repeatedly:
  delay(1);                      // wait for a second
//  tsp.getRegisters(0x00,0x38,adc);
//  printBytes(adc,0x0d);
//  Serial.println();
  tsp.readInfo();
//  tsp.interpretReply();
//  tsp.getRegisters(0x00, 0x0d, adc)
}
