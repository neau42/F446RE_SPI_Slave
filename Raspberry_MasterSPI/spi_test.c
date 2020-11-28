/* COMPILE:
 * gcc -Wall -Wextra -Werror -o spi spi_test.c
 *
 * USAGE:
 * $>./spi /dev/spidev0.1 100000
 *
 * $> yes "read" | ./spi /dev/spidev0.1 100000
 *
 * $> yes "send
        42" | ./spi /dev/spidev0.1 100000
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

int ft_send(char *line, int fd_spi)
{
    int value;
    unsigned char byte;

    printf("\tvalue: ");
    fgets(line, 80, stdin);
    if (sscanf(line, "%d", & value) != 1)
    {
           fprintf(stderr, "integer value expected\n\r");
            return (-1);
    }
    byte = (unsigned char) (value & 0xFF);
    if (write(fd_spi, & byte, 1) != 1)
    {
            perror("write");
            exit(EXIT_FAILURE);
    }
    return(0);
}

void ft_read(int fd_spi)
{
    unsigned char byte;

    if ((read(fd_spi, & byte, 1)) <= 0)
    {
            perror("write");
            exit(EXIT_FAILURE);
    }
    fprintf(stdout, "receive: %d\n\r", byte);
}

int spi_open(char *file_name , char *str_speed)
{
    int fd_spi;
    unsigned int speed = 250000;


    fd_spi = open(file_name, O_RDWR);
    if (fd_spi < 0)
    {
        perror(file_name);
        return (-1);
    }

    if (sscanf(str_speed, "%d", & speed) != 1)
    {
        fprintf(stderr, "Wrong value for speed: %s\n\r", str_speed);
        return (-1);
    }
    if (ioctl(fd_spi, SPI_IOC_WR_MAX_SPEED_HZ, & speed) != 0)
    {
        perror("ioctl");
        return (-1);
    }
    return (fd_spi);
}

int main(int argc, char *argv[])
{
        int fd_spi;
        char line[80];

        if (argc != 3) {
                fprintf(stderr, "usage: %s <spi-port> <spi-speed>\n\r", argv[0]);
                exit(EXIT_FAILURE);
        }
        if ((fd_spi = spi_open(argv[1], argv[2])) < 0)
            exit(EXIT_FAILURE);
        while (printf("Commande: ")
                && fgets(line, 80, stdin) != NULL)
        {
                line[strlen(line) - 1] = '\0';
                if (strcmp(line, "send") == 0)
                {
                    if (ft_send(line, fd_spi) == -1)
                        continue;
                }
                else if (strcmp(line, "read") == 0)
                    ft_read(fd_spi);
                else 
                    printf("wait 'send' or 'read'\n\r");
                
        }
        close(fd_spi);
        return EXIT_SUCCESS;
}
