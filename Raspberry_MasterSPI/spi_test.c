/* COMPILE:
 * gcc -Wall -Wextra -Werror -o spi spi_test.c
 *
 * USAGE:
 * ./spi /dev/spidev0.1 100000
 * 
 */

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>

int main(int argc, char *argv[])
{
        int fd_spi;
        char line[80];
        int value;
        unsigned char byte;

        unsigned int speed = 250000;

        if (argc != 3) {
                fprintf(stderr, "usage: %s <spi-port> <spi-speed>\n\r", argv[0]);
                exit(EXIT_FAILURE);
        }
        fd_spi = open(argv[1], O_RDWR);
        if (fd_spi < 0) {
                perror(argv[1]);
                exit(EXIT_FAILURE);
        }

        if (sscanf(argv[2], "%d", & speed) != 1) {
                fprintf(stderr, "Wrong value for speed: %s\n\r", argv[2]);
                exit(EXIT_FAILURE);
        }
        if (ioctl(fd_spi, SPI_IOC_WR_MAX_SPEED_HZ, & speed) != 0) {
                perror("ioctl");
                exit(EXIT_FAILURE);
        }

        while (fgets(line, 80, stdin) != NULL) {
                line[strlen(line) - 1] = '\0';

                printf("get: {%s}\n\r", line);

                if (strcmp(line, "send") == 0)
                {
                        printf("value: ");
                        fgets(line, 80, stdin);
                        if (sscanf(line, "%d", & value) != 1)
                        {
                               fprintf(stderr, "integer value expected\n\r");
                                continue;
                        }
                        byte = (unsigned char) (value & 0xFF);
                        printf("Send: '%d'\n", byte);
                        if (write(fd_spi, & byte, 1) != 1)
                        {
                                perror("write");
                                exit(EXIT_FAILURE);
                        }
                }
                else if (strcmp(line, "read") == 0)
                {
                        if ((read(fd_spi, & byte, 1)) <= 0)
                        {
                                perror("write");
                                exit(EXIT_FAILURE);
                        }
                    fprintf(stdout, "receive: %d", byte);
                }
                else 
                        printf("wait 'send' or 'read'\n\r");


                
        }
        close(fd_spi);
        return EXIT_SUCCESS;
}
