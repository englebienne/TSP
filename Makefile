# board=esp32:esp32:featheresp32
# board=esp32:esp32:esp32da
board=esp32:esp32:esp32

all:
	arduino-cli compile -b $(board) testTSP.ino
	arduino-cli upload -l serial -p /dev/ttyUSB0 -b $(board) testTSP.ino

upload:
	arduino-cli upload -l serial -p /dev/ttyUSB0 -b $(board) testTSP.ino

# TSP.o: TSP.h TSP.ino
# 	arduino-cli compile -b esp32:esp32:featheresp32 TSP.ino


feather:
	arduino-cli compile -b esp32:esp32:adafruit_feather_esp32_v2 testTSP.ino
	arduino-cli upload -l serial -p /dev/ttyUSB0 -b esp32:esp32:adafruit_feather_esp32_v2 testTSP.ino

