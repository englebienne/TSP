#!/usr/bin/env python

import serial,struct
import json
import sys
import time
import cv2
import numpy as np

filename = "output.json"
if len(sys.argv) > 1:
    filename = sys.argv[1]


data = []
with open(filename,"r") as f:
    data = json.load(f)

    
t = data[0]["time"]-1
tot = np.zeros((27,19))
for i,d in enumerate(data):
    # print(int(200*(d["time"]-t)))
    if d["sep"]:
        tot = np.zeros((27,19))
        print(d["label"])
    frame = np.clip(4*(np.array(d["frame"])-20),0,255)
    # print(int(100*(d["time"]-t)))
    tot = np.maximum(tot,frame)
    cv2.imshow("frame",255-cv2.resize(tot,(20*27,20*19)).astype(np.uint8))
    k = chr(cv2.waitKey(int(200*(d["time"]-t))) & 0xff)
    if k == 'q':
        exit(0)
    elif k>='0' and k<='9':
        print("Relabelling frame %d" % i)
        j=i
        while j>=0 and not data[j]["sep"]:
            data[j]["label"] = k
            j -= 1
        if j>=0:
            data[j]["label"] = k
        j=i            
        while j<len(data) and not data[j]["sep"]:
            data[j]["label"] = k
            j += 1
        with open("relabeled.json","w") as f:
            json.dump(data,f,indent=0)
        
    t = d["time"]
        
