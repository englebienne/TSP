#include <Wire.h>
#include "TSP.h"


TSP tsp;

void isr() {
     tsp.handleInterrupt();
}

void setup() {
  Serial.begin(921600, SERIAL_8N1);
  while (!Serial) 
    delay(10); // wait for serial port to connect. Needed for native USB
  
  Serial.printf("\nStarting connection with TSP\n");
  tsp.init();

  pinMode(IRQ_PIN,OUTPUT);
  digitalWrite(IRQ_PIN,HIGH);
  pinMode(IRQ_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(IRQ_PIN), isr, FALLING);
  
  tsp.printVersion();
}



void loop() {
  delay(1);                      // wait for a second
  tsp.readInfo();
}
