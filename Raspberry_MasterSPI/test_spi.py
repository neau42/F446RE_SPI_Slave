

import sys
import time
import spidev


spi = spidev.SpiDev()
spi.open(0, 1)
spi.max_speed_hz = 10000000 #? 

# sec = int(time.time())
# old_sec = sec
# cmpt_img = 0
# ret = 1;
try:
    while(True):
        ret = spi.readbytes(1)
        print("receive : [", ret[0], "]") #, (send [", ret[0] + 1, "])"
        # ret[0] = ret[0] +1
        # spi.writebytes(ret)
        # time.sleep(.5)
        # sec = int(time.time())
        # if sec != old_sec:
        #     print cmpt_img , "/ s"
        #     old_sec = sec
        #     cmpt_img = 0
        # cmpt_img = cmpt_img +1



except KeyboardInterrupt:
    spi.close()
    sys.exit(0)


