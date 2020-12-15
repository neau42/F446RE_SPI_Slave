import numpy as np
import cv2
import sys
import time

import spidev


## DEFINE
DELAY = 0.5
# DELAY = 0.01
WIDTH = 640
HEIGHT = 480

COEFF = 0.05

UPPER_CROP = 379
LOWER_CROP = 480

CMD_R_SLAVE = [40,41,42,43,44,45,46,47]
CMD_W_SLAVE = [44,43,42,41,40,39,38,37]

## GLOBAL
print_img = False

def getresult(spi):
    # return ()
    time.sleep(DELAY)
    spi.writebytes(CMD_W_SLAVE);
    time.sleep(DELAY)

    ret = spi.readbytes(8)
    if ret != [1,2,3,4,5]:
        print("READ: ", ret)

def motorLeft(spi, delta):
    # print("LEFT: " , delta);
    val = int(delta);
    rest = int((delta - val) * 100)
    # print("delta: ",delta, " int: ", val, ",", rest)
    print("xfer2 [0 0", val, rest,"0] LEFT")
    # spi.writebytes([0,0, val, rest, 0]);# &0xFF, 0, (delta >> 8) &0xFF])
    ret = spi.xfer2([0, 0, val, rest, 0, 0, 0, 0]);# &0xFF, 0, (delta >> 8) &0xFF])

    print("\t RET: <", ret, ">")
    getresult(spi)
    return

def motorRight(spi, delta):
    # print("RIGHT: " , delta);

    val = int(delta);
    rest = int((delta - val) * 100)
    # print("delta: ",delta, " int: ", val, ",", rest)
    print("xfer2 [1 1", val, rest,"1] RIGHT")
    # spi.writebytes([1,1, val, rest, 1]);# &0xFF, 0, (delta >> 8) &0xFF])
    ret = spi.xfer2([1,1, val, rest, 1, 1, 1, 1]);# &0xFF, 0, (delta >> 8) &0xFF])
    print("\t RET: <", ret, ">")
    # xfer2
    getresult(spi)
    return

def init_spi():
    spi = spidev.SpiDev()
    print("spi init done")
    spi.open(0, 1)
    print("spi fd open")

    spi.max_speed_hz = 250000 #? 
    print("spi speed set")

    # spi.writebytes([0,1,2,3,4])
    # print("spi write")

    return(spi)

def init_video_capture():
    video_capture = cv2.VideoCapture(0)
    video_capture.set(3, WIDTH)
    video_capture.set(4, HEIGHT)
    return (video_capture)

def capture_frame(video_capture):
    # Capture the frames
    ret, frame = video_capture.read()
    # print("ret_h: ", ret_h,"ret_s: ", ret_s,"ret_v: ", ret_v)
    if print_img:
        name = "0_init_" + str(sec) +"_" + str(i) + ".jpg"
        cv2.imwrite(name, frame)
    return (frame)

def convert_frame(frame):
    # Crop the image
    crop_img = frame[UPPER_CROP:LOWER_CROP, 0:WIDTH]
    if print_img:
        cv2.imwrite('1_crop.jpg', crop_img)
    # Convert to grayscale
    gray = cv2.cvtColor(crop_img, cv2.COLOR_BGR2GRAY)
    if print_img:
        cv2.imwrite('2_gray.jpg', gray)
    # Gaussian blur # necessaire?
    blur = cv2.GaussianBlur(gray,(5,5),0)
    if print_img:
        cv2.imwrite('3_blur.jpg', blur)
    # Color thresholding
    ret,thresh = cv2.threshold(blur, 60, 255, cv2.THRESH_BINARY_INV)
    if print_img:
        cv2.imwrite('4_thresh.jpg', thresh)
    return (thresh)

def find_line_center(thresh):
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
            cv2.imwrite(name_img, crop_img)
            # sys.exit(0)
        return (cx)
    return (0)

def find_line(spi):
    video_capture = init_video_capture()
    sec = int(time.time())
    old_sec = sec
    img = 0
    cmpt_img = 0
    i = 0
    try:
        while(True):
            i = i + 1
            if sec != old_sec:
                print(cmpt_img , "img/s")
                old_sec = sec
                cmpt_img = 0
                i = 0
                    
            frame = capture_frame(video_capture)
            thresh = convert_frame(frame)
            cx = find_line_center(thresh)

            if (cx != 0):
                delta = COEFF * (cx - 320)
                sec = int(time.time())
                cmpt_img = cmpt_img +1
                # print("spi.writebytes(CMD_R_SLAVE)")
                spi.writebytes(CMD_R_SLAVE);
                time.sleep(DELAY)
                motorRight(spi, -delta)
                time.sleep(DELAY)
                # print("spi.writebytes(CMD_R_SLAVE)")
                spi.writebytes(CMD_R_SLAVE);
                time.sleep(DELAY)
                motorLeft(spi, delta)
            else:
                print("No contour detected.. retry")

    except KeyboardInterrupt:
        spi.close()
        sys.exit(0)


def main():
    if (len(sys.argv) > 1) and (sys.argv[1] == '-i'):
        print_img = True
    spi = init_spi()
    find_line(spi)

main()