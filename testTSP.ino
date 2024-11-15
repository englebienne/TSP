#include <Wire.h>
#include "TSP.h"


int ready = 0; 
hw_timer_t *My_timer = NULL;
void IRAM_ATTR onTimer(){
     ready++;
}


TSP tsp;


void setup() {
     Serial.begin(921600, SERIAL_8N1);
     // Serial.begin(500000, SERIAL_8N1);
     while (!Serial) 
          delay(10);

     delay(1000);
//     Serial.printf("ESP32 reboot");
     Serial.printf("\n  +++ Touch-Sensitive Patch Driver Version %s\n", tsp.getVersion());
     Serial.printf("  +++ Resolution %s\n", tsp.getResolution());
     
     tsp.init();
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
