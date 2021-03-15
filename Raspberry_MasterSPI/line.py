#! /usr/bin/python3

#* ************************************************************************** *#
#*                                                                            *#
#*                                                        :::      ::::::::   *#
#*   line.c                                             :+:      :+:    :+:   *#
#*                                                    +:+ +:+         +:+     *#
#*   By: boulayen                                   +#+  +:+       +#+        *#
#*        noe.boulaye@univ-grenoble-alpes.fr      +#+#+#+#+#+   +#+           *#
#*   Created: 2020/11/15 10:42:11 by nboulaye          #+#    #+#             *#
#*   Updated: 2021/01/20 09:33:42 by nboulaye         ###   ########.fr       *#
#*                                                                            *#
#* Execution:                                                                 *#
#* $>python3 line.py                                                          *#
#*                                                                            *#
#* $>python3 line.py -i # Print image: start the loop once and save the images*#
#*                                      + test different thresholds values    *#
#*                                                                            *#
#* $>python3 line.py -d # Debug: save one image by second                     *#
#*                                                                            *#
#* ************************************************************************** *#

import cv2
import sys
import time
import spidev

WIDTH = 640
HEIGHT = 480

UPPER_CROP = 379
LOWER_CROP = HEIGHT

R_W_CMD = 0
CMD_R_SLAVE = [40,41,42,43]
CMD_W_SLAVE = [43,42,41,40]

VALID_RETURN = [1,2,3,4]

SPI_MAX_SPEED = 250000
THRESHOLD = 80
DELAY = 0.01
MOTOR_MAX_SPEED = 70

def initSPI():
    print("spi init... ", end="")
    spi = spidev.SpiDev()
    spi.open(0, 1)
    spi.max_speed_hz = SPI_MAX_SPEED
    print("Done")
    return(spi)

def initVideoCapture():
    video_capture = cv2.VideoCapture(0)
    video_capture.set(3, HEIGHT)
    video_capture.set(4, WIDTH)
    return (video_capture)

def getResult(spi):
    if R_W_CMD == 1:
        spi.writebytes(CMD_W_SLAVE)
        time.sleep(DELAY)
    ret = spi.readbytes(4)
    while (ret != VALID_RETURN):
        print("read err [4]: ", ret , " != ", VALID_RETURN)
        time.sleep(DELAY)
        while (ret[0] != VALID_RETURN[3]):
            print("read err [1] : ", ret[0], " != ", VALID_RETURN[3])
            ret = spi.readbytes(1)
            time.sleep(DELAY)
        ret = spi.readbytes(4)

def sendMotorsValues(spi, delta_left, delta_right):
    if R_W_CMD == 1:
        spi.writebytes(CMD_R_SLAVE)
        time.sleep(DELAY)
    print("Send: LEFT:" , delta_left, "RIGHT:" , delta_right, "(" , [0, int(delta_left), int(delta_right), 0], ")")
    spi.writebytes([0, int(delta_left), int(delta_right), 0])
    time.sleep(DELAY)
    getResult(spi)

def captureFrame(video_capture):
    ret, frame = video_capture.read()
    if print_img:
        name = "img/" + str(sec) + "_0_init.jpg"
        cv2.imwrite(name, frame)
    return (frame)


def test_threshold(blur):
    for i in range(10,255, 5):
        _, thresh = cv2.threshold(blur, i, 255, cv2.THRESH_BINARY_INV)
        ret = cv2.imwrite("img/test_threshold_" + str(i) +".jpg", thresh)

def convertFrame(frame):
    global crop_img

    rot = cv2.rotate(frame, cv2.cv2.ROTATE_90_CLOCKWISE) 
    crop_img = rot[UPPER_CROP:LOWER_CROP, 0:WIDTH]
    gray = cv2.cvtColor(crop_img, cv2.COLOR_BGR2GRAY)
    blur = cv2.GaussianBlur(gray, (5, 5), 0)

    if print_img:
        test_threshold(blur)

    _, thresh = cv2.threshold(blur, THRESHOLD, 255, cv2.THRESH_BINARY_INV)
    if print_img:
        name = "img/" + str(sec)
        cv2.imwrite(name + "_1_rotate.jpg", rot)
        cv2.imwrite(name + "_2_crop.jpg", crop_img)
        cv2.imwrite(name + "_3_grey.jpg", gray)
        cv2.imwrite(name + "_4_blur.jpg", blur)
        cv2.imwrite(name + "_5_thresh.jpg", thresh)
    return (thresh)

def saveContourImage(contours, moments, cx, name):
        cy = int(moments['m01'] / moments['m00'])
        cv2.line(crop_img, (cx, 0), (cx, 720), (255, 0, 0), 1)
        cv2.line(crop_img, (0, cy), (1080, cy), (0, 0, 255), 1)
        cv2.drawContours(crop_img, contours, -1, (0, 255, 0), 1)
        cv2.putText(crop_img, str(cx), (25, 80), cv2.FONT_HERSHEY_SIMPLEX, 1, (10, 10, 10), 1, cv2.LINE_AA)
        cv2.imwrite(name, crop_img)

def saveImg(contours, moments, cx):
    if print_img:
        name = "img/" + str(sec) + "_6_contour_" + str(cx) + ".jpg"
        saveContourImage(contours, moments, cx, name)
        sys.exit(0)
    elif sec != old_sec and dbg:
        name = "img/" + str(sec) + "_Contour_" + str(cx) + ".jpg"
        saveContourImage(contours, moments, cx, name)

def findLineCenter(thresh):
    contours, hierarchy = cv2.findContours(thresh, 1, cv2.CHAIN_APPROX_NONE)
    if len(contours) == 0:
        return (0)
    c = max(contours, key = cv2.contourArea)
    moments = cv2.moments(c)
    if int(moments['m00']) == 0:
        return (0)
    cx = int(moments['m10'] / moments['m00'])
    saveImg(contours, moments, cx)
    return (cx)

def computeMotorValue(spi, delta):
    sendMotorsValues(spi, (MOTOR_MAX_SPEED) / 2 + int(delta / 2), (MOTOR_MAX_SPEED) / 2 - int(delta / 2))
    time.sleep(DELAY)

def rotateRobot(spi):
    sendMotorsValues(spi, -MOTOR_MAX_SPEED / 2, MOTOR_MAX_SPEED / 2)
    time.sleep(DELAY)

def loop(spi):
    global sec
    global old_sec

    video_capture = initVideoCapture()
    sec = int(time.time())
    old_sec = sec
    cmpt_img = 0
    error_counter = 0
    try:
        while (True):
            sec = int(time.time())
            frame = captureFrame(video_capture)
            thresh = convertFrame(frame)
            cx = findLineCenter(thresh)
            if (cx != 0):
                error_counter = 0
                delta = (cx - (WIDTH / 2)) / (WIDTH / 2) * (MOTOR_MAX_SPEED / 2)
                cmpt_img += 1
                computeMotorValue(spi, delta)
            else:
                error_counter += 1
                if (error_counter > 5):
                    rotateRobot(spi)
                print("No contour detected.. retry (",error_counter,")", end="")
            if sec != old_sec:
                print(cmpt_img , "img/s")
                old_sec = sec
                cmpt_img = 0
    except KeyboardInterrupt:
        spi.close()
        sys.exit(0)

def main():
    global print_img
    global dbg

    dbg = False
    print_img = False
    if (len(sys.argv) > 1) and (sys.argv[1] == '-i'):
        print_img = True
    elif (len(sys.argv) > 1) and (sys.argv[1] == '-d'):
        dbg = True
    spi = initSPI()
    loop(spi)

main()
