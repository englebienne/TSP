#!/usr/bin/env python

import serial,struct
import sys
import time
import cv2
import numpy as np

NUM_ROW = 27
NUM_COL = 19

portname = "/dev/ttyUSB0"
if len(sys.argv) > 1:
    portname = sys.argv[1]


def resync(ser):
    print("Re-syncing stream")
    while True:
        buf = ser.readline()        
        # for b in buf:
        #     if printable(b):
        #         print(chr(b),end='')
        try:
            l = buf.decode()
            if l[-6:] == "FRAME\n":
                print("Start of frame")
                break
                                  
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
    length = NUM_ROW*NUM_COL + 1 # one byte per measurement, plus newline character
    res = ser.read(length)       # Read the whole thing
    length -= len(res)           # If the serial port was slow
    while length != 0:           # keep reading more, until everything is read
        l = ser.read(length)
        length -= len(l)
        res += l

    # for b in res:
    #     if printable(b):
    #         print(chr(b),end='')
                                  
    return res


start = time.time()
N=0;
while True:
    with serial.Serial(portname, 921600, timeout=1) as ser:
        resync(ser)
        rows,cols = NUM_ROW,NUM_COL
        img = np.zeros((rows,cols))

        while True:
            N+=1
            # print(N,time.time()-start,N/(time.time()-start))
            l = readFrame(ser)
            i=0
            for r in range(NUM_ROW):
                for c in range(NUM_COL):
                    img[r][c] = 1.5*(l[i]-3)
                    i+=1

            tmp = cv2.resize(cv2.rotate(np.clip(img,0,255), cv2.ROTATE_180),(rows*30,cols*40))
            cv2.imshow("test", tmp.astype(np.uint8))
            k = chr(cv2.waitKey(1) & 0xff)
            if k ==  'q':
                exit(0)
            if k == 'c' or k == 'r':
                ser.write(bytes([1]));
                r = 0
                continue

            l = ser.read(6)
            if l.decode() != "FRAME\n":
                print("Lost sync '%s'" % (l.decode()))
                resync(ser)

            
            


