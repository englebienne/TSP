#include <Wire.h>
#include "TSP.h"


int ready = 0; 
hw_timer_t *My_timer = NULL;
void IRAM_ATTR onTimer(){
     ready++;
}


TSP tsp;


void setup() {
     // My_timer = timerBegin(1, 80, true);
     // timerAttachInterrupt(My_timer, &onTimer, true);
     // timerAlarmWrite(My_timer, 20000, true);
     // timerAlarmEnable(My_timer); //Just Enable     

     Serial.begin(921600, SERIAL_8N1);
     while (!Serial) 
          delay(10);
  
     Serial.printf("\nStarting connection with TSP\n");
     tsp.init();
}



int count = 0;
void loop() {
     delayMicroseconds(1);                      
     tsp.readInfo();
     // count++;
     // if (ready) {
     //      // Serial.printf("count=%d\n", count);
     //      tsp.transmit();
     //      ready=0;
     // }
     // if (count > 100) {
     //      count = 0;
     //      tsp.transmit();
     // }

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


// void app_main(void) {
//      setup();
//      while (true) {
//           loop();
//      }
// }
