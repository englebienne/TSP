all:
	arduino-cli compile -b esp32:esp32:featheresp32 testTSP.ino
	arduino-cli upload -l serial -p /dev/ttyUSB0 -b esp32:esp32:featheresp32 testTSP.ino

# TSP.o: TSP.h TSP.ino
# 	arduino-cli compile -b esp32:esp32:featheresp32 TSP.ino


