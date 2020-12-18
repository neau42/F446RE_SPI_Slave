/* spi_master_write.c - Programme pour Raspberry Pi 
 * (from: https://www.blaess.fr/christophe/2012/11/02/spi-sur-raspberry-pi-1/) 
 * 
 * COMPILE:
 * gcc -o spi_read spi_master_read.c
 *
 * USAGE:
 * ./spi_write /dev/spidev0.1 100000
 * 
 * 
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux/types.h>
#include <linux/spi/spidev.h>

#include <sys/ioctl.h>

int main(int argc, char *argv[])
{
        int fd_spi;
        char ligne[80];
        int value;
        unsigned char byte[128] = {0};
        unsigned int speed = 250000;
        int read_value = 0;

        if (argc != 3) {
                fprintf(stderr, "usage: %s <spi-port> <spi-speed>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
        fd_spi = open(argv[1], O_RDWR);
        if (fd_spi < 0) {
                perror(argv[1]);
                exit(EXIT_FAILURE);
        }

        if (sscanf(argv[2], "%d", & speed) != 1) {
                fprintf(stderr, "Wrong value for speed: %s\n", argv[2]);
                exit(EXIT_FAILURE);
        }
        if (ioctl(fd_spi, SPI_IOC_WR_MAX_SPEED_HZ, & speed) != 0) {
                perror("ioctl");
                exit(EXIT_FAILURE);
        }
        while (1) {
		printf("read 127 byte\n\r");
                if ((read_value = read(fd_spi, & byte, 127)) <=0)
                {
                        perror("write");
                        exit(EXIT_FAILURE);
                }
                printf("received: [%s] (%d)\n\r", byte, read_value);

        }
        close(fd_spi);
        return EXIT_SUCCESS;
}
