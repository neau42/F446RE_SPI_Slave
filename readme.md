# test SPI sur STM3F446RE(slave) / Raspberry(master)

SPI | Carte Master | Carte Slave | 
| --- | --- | --- |
ground | pin 25		| GND
SPI2_MOSI| pin 19		|	PB15
SPI2_SCK | pin 23	|	PB13

####  stm:
###### compilation: 
    "make" à la racine du dossier
###### televerser sur stm32:
    copie de build/F446RE_Slave.bin sur le peripherie émulé sur ordi
###### lecture port serie:
    screen /dev/tty.usbmodem**** 115200

##### Raspberry:
###### compilation:
    gcc -o spi_write spi_master_write.c
###### execution:
    ./spi_write /dev/spidev0.1 100000

    ou 
    
    python line.py

