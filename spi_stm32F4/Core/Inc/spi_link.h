#ifndef SPI_LINK__H
# define SPI_LINK__H
# include "Lib_PedagoBot_TP9.h"
# include <stdio.h>
# include <string.h>

# define R_W_CMD 0

extern SPI_HandleTypeDef hspi2;

void    spiLoop(void);
void    cmdLeftMotor(int8_t value);
void    cmdRightMotor(int8_t value);
void    stopMotors(void);
void    spiRead(uint8_t *buffer_read);
void    spiWrite(uint8_t *buffer_send);
void    readWriteWithValidation(uint8_t *buffer_read, uint8_t *buffer_send);
uint8_t blueButtonPressed(void);
void    waitPressBlueButton(void);
void    spiError(uint8_t *buffer_read, uint8_t *buffer_send);


#endif
