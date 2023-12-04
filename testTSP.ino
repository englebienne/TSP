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

void isr() {
     tsp.handleInterrupt();
}

void setup() {
     Serial.begin(921600, SERIAL_8N1);
     while (!Serial) 
          delay(10);
  
     Serial.printf("\nStarting connection with TSP\n");
     tsp.init();

     // pinMode(IRQ_PIN,OUTPUT);
     // digitalWrite(IRQ_PIN,HIGH);
     // pinMode(IRQ_PIN, INPUT_PULLUP);
     // attachInterrupt(digitalPinToInterrupt(IRQ_PIN), isr, FALLING);
  
     // tsp.printVersion();
}



void loop() {
     delay(1);                      // wait for a second
     tsp.readInfo();
}


// void app_main(void) {
//      setup();
//      while (true) {
//           loop();
//      }
// }
