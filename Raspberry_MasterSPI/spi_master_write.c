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
        unsigned char byte;

        unsigned int speed = 250000;

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

        while (fgets(ligne, 80, stdin) != NULL) {
                if (sscanf(ligne, "%d", & value) != 1) {
                        fprintf(stderr, "integer value expected\n");
                        continue;
                }
                byte = (unsigned char) (value & 0xFF);
		printf("value SEND: '%d' (%%d)\n", byte);
                if (write(fd_spi, & byte, 1) != 1) {
                        perror("write");
                        exit(EXIT_FAILURE);
                }
        }
        close(fd_spi);
        return EXIT_SUCCESS;
}
