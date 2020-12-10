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
# define BUFF_SIZE 32

// struct spi_ioc_transfer {
//     __u64       tx_buf;
//     __u64       rx_buf;

//     __u32       len;
//     __u32       speed_hz;

//     __u16       delay_usecs;
//     __u8        bits_per_word;
//     __u8        cs_change;
//     __u32       pad;
// };
 ssize_t spi_transfer(int fd_spi)
 {
    struct spi_ioc_transfer buff;
    char buff_tx[BUFF_SIZE];// = {42};
    char buff_rx[BUFF_SIZE] = {0};

    memset(&buff, 0, sizeof(buff));
    memset(buff_tx, 0x2a, 8);


    printf("send: %d\n\r", buff_tx[0]);

    buff.rx_buf = (__u64)(long)buff_rx; //~
    buff.tx_buf = (__u64)(long)buff_tx; //~

    buff.len = 1;
    buff.delay_usecs = 0;
    if(ioctl(fd_spi, SPI_IOC_MESSAGE(1), &buff) < 0)
        return (-1);
    printf("receive: %d\n\r", buff_rx[0]);

    return (buff.len);
 }


int ft_send(char *line, int fd_spi)
{
    // (void)line;
    // return (spi_transfer(fd_spi));

    int motor;
    float value;
    // unsigned char byte;
    unsigned char buffer[3] = {0};
    printf("\tmotor: (0:right||1:left) ");

    fgets(line, 80, stdin);
    if (sscanf(line, "%d", & motor) != 1 || motor > 1)
    {
        fprintf(stderr, "integer value < 2 expected\n\r");
        return (-1);
    }
    buffer[0] = motor;
    printf("\tvalue: (%%f) ");
    fgets(line, 80, stdin);
    if (sscanf(line, "%f", & value) != 1)
    {
        fprintf(stderr, "float value expected\n\r");
        return (-1);
    }

    buffer[1] = (int)value;
    // buffer[2] = (buffer[1] - value) *100;
    value -= buffer[1];
    buffer[2] = (int)(value * 100);
    printf("buf[0]= %d, buf[1]= %d, buf[2]= %d, value: %f\n\r", buffer[0], buffer[1], buffer[2], value);
    // byte = (unsigned char) (value & 0xFF);
    
    if (write(fd_spi, buffer, 3) != 3)
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
    if(!err)
        err = ioctl(fd_spi, SPI_IOC_RD_MODE32, &mode);
    if (!err)
        err = ioctl(fd_spi, SPI_IOC_WR_MODE32, &mode);
    if(!err)
        err = ioctl(fd_spi, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if(!err)
        err = ioctl(fd_spi, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if(!err)
        err = ioctl(fd_spi, SPI_IOC_WR_MAX_SPEED_HZ, &speed);//send 'write speed' before read 'read speed'
    if(!err)
        err = ioctl(fd_spi, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

    printf("mode: %d\n", mode);
    printf("bits: %d\n", bits);
    printf("speed: %d\n", speed);
    if(!err)
        return (fd_spi);
    perror("ioctl");
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
