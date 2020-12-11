import numpy as np
import cv2
import sys
#from datetime import datetime
import time

import spidev

def getresult():
    print("wait result...")

    # ret = spi.readbytes(1)
    # while ret[0] != 42:
    #     print "result error ?! : ", ret
    #     ret = spi.readbytes(1)
    #     time.sleep(.2)
    # print "result ok"

def motorLeft(delta):
    val = int(delta);
    rest = int((delta - val) * 100)
    # print("int: ", val, " , ", rest)
    spi.writebytes([0,0, val, rest, 0]);# &0xFF, 0, (delta >> 8) &0xFF])
    time.sleep(0.01)
    return

def motorRight(delta):
    val = int(delta);
    rest = int((delta - val) * 100)
    # print("int: ", val, " , ", rest)
    spi.writebytes([1,1, val, rest, 1]);# &0xFF, 0, (delta >> 8) &0xFF])
    time.sleep(0.01)
    return

print_img = False

if (len(sys.argv) > 1) and (sys.argv[1] == '-i'):
    print_img = True

# image size
WIDTH = 640
HEIGHT = 480

# turn coeff
COEFF = 0.05
# base robot speed in straight line
# SPEED = 30

video_capture = cv2.VideoCapture(0)
video_capture.set(3, WIDTH)
video_capture.set(4, HEIGHT)

sec = int(time.time())
old_sec = sec
img = 0
cmpt_img = 0

spi = spidev.SpiDev()
print("spi init done")
spi.open(0, 1)
print("spi fd open")

spi.max_speed_hz = 250000 #? 
print("spi speed set")

spi.writebytes([0,1,2,3,4])
print("spi write")

try:
    while(True):

        if sec != old_sec:
            print(cmpt_img , "img/s")
            old_sec = sec
            cmpt_img = 0
        #print("LOOP : ", datetime.now().time())
        # Capture the frames
        ret, frame = video_capture.read()
        if print_img:
            cv2.imwrite('0_init.jpg', frame)

        # Crop the image
        # Keep the 100 lower pixels
        crop_img = frame[379:480, 0:640]
        if print_img:
            cv2.imwrite('1_crop.jpg', crop_img)

        # Convert to grayscale
        gray = cv2.cvtColor(crop_img, cv2.COLOR_BGR2GRAY)
        if print_img:
            cv2.imwrite('2_gray.jpg', gray)

        # Gaussian blur
        blur = cv2.GaussianBlur(gray,(5,5),0)
        if print_img:
            cv2.imwrite('3_blur.jpg', blur)

        # Color thresholding
        ret,thresh = cv2.threshold(blur,60,255,cv2.THRESH_BINARY_INV)
        if print_img:
            cv2.imwrite('4_thresh.jpg', thresh)

        # Find the contours of the frame
        _, contours,hierarchy = cv2.findContours(thresh, 1, cv2.CHAIN_APPROX_NONE)

        # Find the biggest contour (if detected)
        if len(contours) > 0:
            c = max(contours, key=cv2.contourArea)
            M = cv2.moments(c)

            # Skip to avoid div by zero
            if int(M['m00']) == 0:
                continue

            # Get the line center
            cx = int(M['m10']/M['m00'])
            cy = int(M['m01']/M['m00'])

            if print_img:
                cv2.line(crop_img,(cx,0),(cx,720),(255,0,0),1)
                cv2.line(crop_img,(0,cy),(1080,cy),(0,0,255),1)
                cv2.drawContours(crop_img, contours, -1, (0,255,0), 1)
                cv2.imwrite('5_contour.jpg', crop_img)
                sys.exit(0)

            delta = COEFF * (cx - 320)
            sec = int(time.time())
            cmpt_img = cmpt_img +1
            motorRight(-delta)
            motorLeft(delta)

except KeyboardInterrupt:
    spi.close()
    sys.exit(0)


