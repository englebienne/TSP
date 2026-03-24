# Touch-Sensitive Patch

Minimal Software implementation of the University of Twentes Module 7 "Hands-on AI" Touch-sensitive Patches. 

## ESP32 Code

The embedded software reads out the information from the MTCH6303 chipset through I2C and makes this available
on the USB serial connection

- TSP.ino: Implementation of the interfacing code
- testTSP.ino: main loop

## Python code

vis.py Example code for reading out and visualising the data from the USB serial connection  

## Case

Code for 3D printing the case for the electronics
