#include <Wire.h>
#include "TSP.h"

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"


TSP tsp;


void setup() {
     Serial.begin(921600, SERIAL_8N1);
     while (!Serial) 
          delay(10);
  
     Serial.printf("\nStarting connection with TSP\n");
     tsp.init();
}



void loop() {
     delayMicroseconds(1);                      // wait for a second
     tsp.readInfo();
}


// void app_main(void) {
//      setup();
//      while (true) {
//           loop();
//      }
// }
