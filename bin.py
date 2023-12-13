#!/usr/bin/env python

import serial,struct
import sys
import time
import cv2
import numpy as np

portname = "/dev/ttyUSB0"
if len(sys.argv) > 1:
    portname = sys.argv[1]


def resync(ser):
    print("Re-syncing stream")
    while True:
        buf = ser.readline()        
        try:
            l = buf.decode()
            print(l)
            if l[-6:] == "FRAME\n":
                print("Start of frame")
                break
            # else:
        #     for c in l:
        #         print(" %d" % (ord(c)),end="")
        except:
            print("Undecodable buffer of length ", len(buf));

def printable(b):
    b = chr(b)
    if b>='a' and b<='z':
        return True
    if b>='A' and b<='Z':
        return True
    if b>='0' and b<='9':
        return True
    return b in ['[',']','.',',','{','}']
            
def readFrame(ser):
    length = 27*19 + 1
    res = ser.read(length)
    length -= len(res)
    while length != 0:
        l = ser.read(length)
        length -= len(l)
        res += l

    # for b in res:
    #     if printable(b):
    #         print(chr(b),end='')
                                  
    return res

    
while True:
    with serial.Serial(portname, 921600, timeout=1) as ser:
        resync(ser)
        rows,cols = 27,19
        img = np.zeros((rows,cols))
        # prev = img.copy()

        while True:
            l = readFrame(ser)
            # print(len(l))
            r=0
            c=0
            for v in struct.iter_unpack("<B",l[:-1]): # strip the \n
                img[26-r][c] = v[0]
                c+=1
                if c==cols:
                    c=0
                    r+=1                            

            # print(img)

            # img[img < 1] = 0.
            # tmp = cv2.resize(cv2.rotate(np.clip((img+prev)/2,0, 255),cv2.ROTATE_180),(r*40,c*40))
            tmp = cv2.resize(np.clip(img, 0,255),(rows*40,cols*40))
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

            l = ser.read(6)
            # print("'%s'"% (l))
            if l.decode() != "FRAME\n":
                print("Lost sync '%s'" % (l.decode()))
                resync(ser)

            
            




            
#     except serial.serialutil.SerialException:
# #        print("Could not open /dev/ttyUSB0")
#         time.sleep(1)
#     except KeyboardInterrupt:
#         exit(0)
#     except UnicodeDecodeError:
#         pass
                    
               
          

    
