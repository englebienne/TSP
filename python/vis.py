#!/usr/bin/env python

import serial
import sys
import time
import cv2
import numpy as np

portname = "/dev/ttyUSB0"
if len(sys.argv) > 1:
    portname = sys.argv[1]

while True:
    with serial.Serial(portname, 921600, timeout=1) as ser:
        while True:
            l = ser.readline().decode()
            if l == "FRAME\n":
                print("Start of frame")
                break
            # else:
            #     for c in l:
            #         print(" %d" % (ord(c)),end="")

        r,c = 27,19
        img = np.zeros((r,c))
        prev = img.copy()
        r = 0
        while True:
            l = ser.readline().decode()
            # print("'%s'"% (l))
            if l == "FRAME\n":
                # img[img < 1] = 0.
                # tmp = cv2.resize(cv2.rotate(np.clip((img+prev)/2,0, 255),cv2.ROTATE_180),(r*40,c*40))
                tmp = cv2.resize(np.clip((img+prev)/2,0, 255),(r*40,c*40))
                prev = img.copy()
                # print(tmp.astype(np.uint8))
                cv2.imshow("test", tmp.astype(np.uint8))
                k = chr(cv2.waitKey(1) & 0xff)
                if k ==  'q':
                    exit(0)
                if k == 'c' or k == 'r':
                    ser.write(bytes([1]));
                # print("Start of frame")
                r = 0
                continue

            try:
                v = np.array([ (int(s,16)-5)/1.2 for s in l.split() ])
                # print(v.min(),v.max(),v.mean())
                # if v.max() > 255:
                #     print(v)                    
                #     v = -(v - (v.max()-255));
                #     print(v)
            except:
                r=0
                continue
            # print(v)
            if len(v) != c:
                r=0
                continue
            # print(r,v)
            img[26-r,:] = v
            r += 1
            
            




            
#     except serial.serialutil.SerialException:
# #        print("Could not open /dev/ttyUSB0")
#         time.sleep(1)
#     except KeyboardInterrupt:
#         exit(0)
#     except UnicodeDecodeError:
#         pass
                    
               
          

    
