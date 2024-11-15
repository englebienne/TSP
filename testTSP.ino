#include <Wire.h>
#include "TSP.h"


int ready = 0; 
hw_timer_t *My_timer = NULL;
void IRAM_ATTR onTimer(){
     ready++;
}


TSP tsp;


void handleFrame(uint8_t *frame, unsigned len) {
     const uint8_t minval = 2;
     
     bool allZero = true;
     for (unsigned i=0; i<len; ++i)
          if (frame[i] > minval) {
               allZero = false;
               break;
          }

     if (allZero) {
          Serial.print("\nFR0");
          return;
     }
     
     
     Serial.print("\nFRAME\n");
     Serial.write(frame,len);
}

void setup() {
     Serial.begin(921600, SERIAL_8N1);
     // Serial.begin(500000, SERIAL_8N1);
     while (!Serial) 
          delay(10);

     delay(1000);
//     Serial.printf("ESP32 reboot");
     Serial.printf("\n  +++ Touch-Sensitive Patch Driver Version %s\n", tsp.getVersion());
     Serial.printf("  +++ Resolution %s\n", tsp.getResolution());

     // Setup I2C connection
     Wire.begin();              // Initialize comunication
     Wire.setClock(410000);     // Can be overclocked a little?
     // Wire.setClock(480000);  // Can be overclocked a little?
     Wire.setTimeout(100);
     
     tsp.registerFrameHandler(handleFrame);

     tsp.init();
     tsp.readInfo();
     tsp.readInfo();
}



void loop() {
     delayMicroseconds(1);                      
     tsp.readInfo();

     if (Serial.available() > 1) {
          byte v = Serial.read();
          switch (v) {
          case 0x01:
               Serial.printf("Recomputing baseline");
               tsp.baseline();
               break;
          }
     }
}
