import numpy as np
import cv2
import sys
import time

import spidev


## DEFINE
DELAY = 0.01
WIDTH = 640
HEIGHT = 480

MAX_SPEED = 40

UPPER_CROP = 379
LOWER_CROP = 480

CMD_R_SLAVE = [40,41,42,43]
CMD_W_SLAVE = [43,42,41,40]

def initSPI():
    spi = spidev.SpiDev()
    print("spi init done")
    spi.open(0, 1)
    print("spi fd open")
    spi.max_speed_hz = 250000 #? 
    print("spi speed set")
    return(spi)

def initVideoCapture():
    video_capture = cv2.VideoCapture(0)
    video_capture.set(3, WIDTH)
    video_capture.set(4, HEIGHT)
    return (video_capture)

def getResult(spi):
    spi.writebytes(CMD_W_SLAVE);
    time.sleep(DELAY)
    ret = spi.readbytes(4)
    while (ret != [1,2,3,4]):
        time.sleep(DELAY)
        ret = spi.readbytes(4)
        print("READ: ", ret)
        
def sendMotorsValues(spi, delta_left, delta_right):
    # print("Send: LEFT:" , delta_left, "RIGHT:" , delta_right)
    # ret = spi.xfer2([0, int(delta_left), int(delta_right), 0])
    spi.writebytes([0, int(delta_left), int(delta_right), 0])
    time.sleep(DELAY)
    getResult(spi)

def captureFrame(video_capture):
    # Capture the frames
    ret, frame = video_capture.read()
    # if print_img:
    #     name = "0_init_" + str(sec) +"_" + ".jpg"
    #     cv2.imwrite(name, frame)
    return (frame)

def convertFrame(frame):
    global crop_img
    # Crop the image
    crop_img = frame[UPPER_CROP:LOWER_CROP, 0:WIDTH]
    # Convert to grayscale
    gray = cv2.cvtColor(crop_img, cv2.COLOR_BGR2GRAY)
    # Gaussian blur # necessaire?
    blur = cv2.GaussianBlur(gray,(5,5),0)
    # Color thresholding
    ret,thresh = cv2.threshold(blur, 60, 255, cv2.THRESH_BINARY_INV)
    return (thresh)

def findLineCenter(thresh):
    # Find the contours of the frame
    _, contours, hierarchy = cv2.findContours(thresh, 1, cv2.CHAIN_APPROX_NONE)
    # Find the biggest contour (if detected)
    if len(contours) > 0:
        c = max(contours, key = cv2.contourArea)
        moments = cv2.moments(c)
        # Skip to avoid div by zero
        if int(moments['m00']) == 0:
            return (0)
        # Get the line center
        cx = int(moments['m10'] / moments['m00'])
        if print_img:
            cy = int(moments['m01'] / moments['m00'])
            cv2.line(crop_img,(cx,0),(cx,720),(255,0,0),1)
            cv2.line(crop_img,(0,cy),(1080,cy),(0,0,255),1)
            cv2.drawContours(crop_img, contours, -1, (0,255,0), 1)
            name_img = "img" + str(cx) + ".jpg"
            print("image:", name_img, "created", "cy:", cy)
            cv2.putText(crop_img, str(cx),(25,80), cv2.FONT_HERSHEY_SIMPLEX, 1, (10, 10, 10), 1, cv2.LINE_AA)
            cv2.imwrite(name_img, crop_img)
        return (cx)
    return (0)

def sendValueSPI(spi, delta):
    spi.writebytes(CMD_R_SLAVE);
    time.sleep(DELAY)
    sendMotorsValues(spi, (MAX_SPEED / 2) + int(delta), (MAX_SPEED / 2) - int(delta))
    time.sleep(DELAY)

def rotate(spi):
    spi.writebytes(CMD_R_SLAVE);
    time.sleep(DELAY)
    sendMotorsValues(spi,-(MAX_SPEED / 2), (MAX_SPEED / 2))
    time.sleep(DELAY)

def findLine(spi):
    video_capture = initVideoCapture()
    global sec
    sec = int(time.time())
    old_sec = sec
    cmpt_img = 0
    error_counter = 0
    try:
        while (True):
            if sec != old_sec:
                print(cmpt_img , "img/s")
                old_sec = sec
                cmpt_img = 0
                i = 0
            frame = captureFrame(video_capture)
            thresh = convertFrame(frame)
            cx = findLineCenter(thresh)

            if (cx != 0):
                error_counter = 0
                delta = (cx - 320) / 320 * (MAX_SPEED / 2) # == [ (cx - 320) / 16 ]  == [ (cx - 320) * 0.0625 ]
                cmpt_img = cmpt_img + 1
                sendValueSPI(spi, delta);
            else:
                error_counter += 1
                if (error_counter > 5):
                    rotate(spi)
                print("No contour detected.. retry (",error_counter,")")
            sec = int(time.time())
    except KeyboardInterrupt:
        spi.close()
        sys.exit(0)
dbg = False

# def wait_slave(spi):
#     ret = [0,0,0,0]
#     print("wait slave ok")
#     try:
#         while (ret != [1,2,3,4]):
#             time.sleep(DELAY)
#             ret = spi.readbytes(4)
#             # print("ret: ", ret)
#         print("done");
#     except KeyboardInterrupt:
#         spi.close()
#         sys.exit(0)

def main():
    global print_img
    print_img = False
    if (len(sys.argv) > 1) and (sys.argv[1] == '-i'):
        print_img = True
    elif (len(sys.argv) > 1) and (sys.argv[1] == '-d'):
        dbg = True
    spi = initSPI()
    # wait_slave(spi)
    findLine(spi)
main()