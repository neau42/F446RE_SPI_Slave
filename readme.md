# test SPI sur STM3F446RE(slave) / Raspberry(master)

SPI | Carte Master | Carte Slave | 
| --- | --- | --- |
ground | pin 25		| GND
SPI2_MOSI| pin 19		|	PB15
SPI2_SCK | pin 23	|	PB13

#### stm32
![alt text](readme/STM32F446RE_SPI.png)


#### raspberry 3
![alt text](readme/Raspberry_3_spi.png)



####  stm:
###### compiler et televerser: 
    /stm32F446RE_spi_slave/ $> make && cp build/F446RE_Slave.bin /Volumes/NODE_F446RE/.
###### lecture port serie:
    ~ $> screen /dev/tty.usbmodem**** 115200

##### Raspberry:
###### compilation:
   /Raspberry_MasterSPI/ $> gcc -o spi_write spi_master_write.c
###### execution:
    /Raspberry_MasterSPI/ $> ./spi_write /dev/spidev0.1 100000

    ou 
    
    /Raspberry_MasterSPI/ $> python line.py

