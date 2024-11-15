#!/usr/bin/env python

import serial
import sys
import time

outfile = None
if len(sys.argv) > 1:
    outfile = open(sys.argv[1], "w+")
portname = "/dev/ttyUSB0"
if len(sys.argv) > 2:
    portname = sys.argv[2]
while True:
    try:
        # with serial.Serial(portname, 500000, timeout=1) as ser:
        with serial.Serial(portname, 921600, timeout=1) as ser:
        # with serial.Serial(portname, 230400, timeout=1) as ser:
            while True:
                l = ser.read().decode()
                print(l, end='')
                if outfile:
                    outfile.write(l)
    except serial.serialutil.SerialException:
#        print("Could not open /dev/ttyUSB0")
        time.sleep(1)
    except KeyboardInterrupt:
        exit(0)
    except UnicodeDecodeError:
        pass
                    
               
          

    
