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
     while (!Serial) 
          delay(10);
  
     Serial.printf("\nStarting connection with TSP\n");
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
