/* COMPILE:
 * gcc -Wall -Wextra -Werror -o spi spi_test.c
 *
 * USAGE:
 * $>./spi /dev/spidev0.1
 *
 * $> yes "read" | ./spi /dev/spidev0.1
 *
 * $> yes "send
        42" | ./spi /dev/spidev0.1
 *
 */

# include <fcntl.h>
# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <unistd.h>
# include <linux/types.h>
# include <linux/spi/spidev.h>
# include <sys/ioctl.h>

# define SPI_BITS_PER_WORD 8
// # define BUFF_SIZE 32
 // ssize_t spi_transfer(int fd_spi)
 // {
 //    struct spi_ioc_transfer buff;
 //    char buff_tx[BUFF_SIZE];// = {42};
 //    char buff_rx[BUFF_SIZE] = {21};

 //    memset(buff_tx, 0x2a, BUFF_SIZE);
 //    memset(&buff, 0, sizeof(buff));
 //    buff.tx_buf = (__u64)buff_tx;
 //    buff.rx_buf = (__u64)buff_rx;
 //    buff.len = BUFF_SIZE;
 //    buff.delay_usecs = 0;
 //    if(ioctl(fd_spi, SPI_IOC_MESSAGE(1), &buff) < 0)
 //        return (-1);
 //    return (buff.len);s
 // }


int ft_send(char *line, int fd_spi)
{
    // (void)line;
    // return (spi_transfer(fd_spi));

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
            perror("read");
            exit(EXIT_FAILURE);
    }
    fprintf(stdout, "receive: %d\n\r", byte);
}

int spi_open(char *file_name)
{
    int fd_spi;
    unsigned int speed = 250000;
    int err = 0;
    // int mode = SPI_MODE_3 ; //SPI_MODE_2 nop; SPI_MODE_1 nop , mode_0 ok send ; SPI_CPOL nope ; .... -_- SPI_MODE_3; // ok read
    int mode = SPI_MODE_0; // ok send 
    int bits = SPI_BITS_PER_WORD;
    
    (void)mode;
    (void)bits;
    fd_spi = open(file_name, O_RDWR);
    if (fd_spi < 0)
    {
        perror(file_name);
        return (-1);
    }
    // if (sscanf(str_speed, "%d", & speed) != 1)
    // {
    //     fprintf(stderr, "Wrong value for speed: %s\n\r", str_speed);
    //     close(fd_spi);
    //     return (-1);
    // }
    printf("mode: %d\n", mode);
    if(!err)
        err = ioctl(fd_spi, SPI_IOC_RD_MODE32, &mode);
    printf("mode: %d\n", mode);
    if (!err)
        err = ioctl(fd_spi, SPI_IOC_WR_MODE32, &mode);
    printf("bits: %d\n", bits);
    if(!err)
        err = ioctl(fd_spi, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if(!err)
        err = ioctl(fd_spi, SPI_IOC_WR_BITS_PER_WORD, &bits);
    printf("bits: %d\n", bits);
    if(!err)
        err = ioctl(fd_spi, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    printf("speed: %d\n", speed);
    if(!err)
        err = ioctl(fd_spi, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if(!err)
        return (fd_spi);
    printf("ERROR\n");
    close(fd_spi);
    return -1;
 }

int main(int argc, char *argv[])
{
        int fd_spi;
        char line[80];

        if (argc != 2)
        {
            fprintf(stderr, "usage: %s <spi-port>\n\r", argv[0]);
            exit(EXIT_FAILURE);
        }
        if ((fd_spi = spi_open(argv[1])) < 0)
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
        return (EXIT_SUCCESS);
}
