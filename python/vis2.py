#!/usr/bin/env python

import serial,struct
import sys
import time
import cv2
import numpy as np
import pygame
from pillow import Pillow

NUM_ROW = 27
NUM_COL = 19
PIX_SIZE = 30;

# pygame setup
pygame.display.init()
screen = pygame.display.set_mode((NUM_COL*PIX_SIZE, NUM_ROW*PIX_SIZE))

portname = "/dev/ttyUSB0"
if len(sys.argv) > 1:
    portname = sys.argv[1]


def printable(b):
    b = chr(b)
    if b>='a' and b<='z':
        return True
    if b>='A' and b<='Z':
        return True
    if b>='0' and b<='9':
        return True
    return b in ['[',']','.',',','{','}']
            

def saveFigure(img):
    filename = cv2.imwrite(time.strftime("%Y-%m-%d.%H:%M:%S.jpg"),img)


def showImg(img):
    for event in pygame.event.get():
        # print(event.type, event)
        if event.type == pygame.QUIT:
            pygame.quit()
        if event.type == pygame.KEYDOWN and event.key == pygame.K_q:
            pygame.quit()

    screen.fill("purple")
    for r in range(NUM_ROW):
        for c in range(NUM_COL):
            v = img[NUM_ROW-1-r,NUM_COL-1-c]
            s = PIX_SIZE
            pygame.draw.rect(screen, (0,v,v), pygame.Rect(c*s,r*s,s-1,s-1))
    pygame.display.flip()
    

pillow = Pillow(NUM_ROW,NUM_COL)
    
if portname.startswith("/dev/tty"):
    touches = len(pillow.touch)
    with serial.Serial(portname, 921600, timeout=1) as ser:
        while True:
            pillow.processLine(ser);
            nt = len(pillow.touch)
            if nt > touches:
                showImg(pillow.touch[-1])
else:
    with open(portname,"br") as f:
        touches = 0
        while True:
            pillow.processLine(f);
            nt = len(pillow.touch)
            if nt > touches:
                showImg(pillow.touch[-1])
        # pillow.processFile(f);
        # print("Touch buffer len",len(pillow.touch))
        # for img in pillow.touch:
        #     showImg(img)
# touches = len(pillow.touch)
# with open(portname,"br") as ser:
#     while True:
#         pillow.processLine(ser);
#         nt = len(pillow.touch)
#         if nt > touches:
#             showImg(pillow.touch[-1])
    
    
        
pygame.quit()
            
            
