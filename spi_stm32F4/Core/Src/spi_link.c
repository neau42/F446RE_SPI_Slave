#include "spi_link.h"

uint8_t 		ButtonPressed = 0;
const uint8_t	CMD_R[4] = {40, 41, 42, 43};
const uint8_t	CMD_W[4] = {43, 42, 41, 40};

void    spiLoop(void)
{
	uint8_t buffer_send[4] = {1, 2, 3, 4};
	uint8_t buffer_read[4] = {0};

	waitPressBlueButton();
	while (1)
	{
		blueButtonPressed();
		if (ButtonPressed == 1)
		{
			if (R_W_CMD)
				readWriteWithValidation(buffer_read, buffer_send);
			else
			{
				spiRead(buffer_read);
				spiWrite(buffer_send);
			}
		}
		else
			stopMotors();
    }
}

void    spiRead(uint8_t *buffer_read)
{
	HAL_SPI_Receive(&hspi2, buffer_read, 4, 250000);
	if (buffer_read[0] == 0 && buffer_read[3] == 0)
	{
//		printf("LEFT: %d ; RIGHT: %d\n\r", (int8_t)buffer_read[1], (int8_t)buffer_read[0]);
		cmdLeftMotor((int8_t)buffer_read[1]);
		cmdRightMotor((int8_t)buffer_read[2]);
	}
	else
		printf("ERROR: [%d %d %d %d] from master\n\r", buffer_read[0], buffer_read[1], buffer_read[2], buffer_read[3]);
}

void    spiWrite(uint8_t *buffer_send)
{
	uint8_t parazite = 0;

	printf("spiWrite send: %d %d %d %d\n\r",buffer_send[0], buffer_send[1], buffer_send[2], buffer_send[3]);
	HAL_SPI_Transmit(&hspi2, buffer_send, 4, 250000);
	// need to read one bite after Transmit ??
	HAL_SPI_Receive(&hspi2, &parazite, 1, 250000);
}

void    cmdLeftMotor(int8_t value)
{
	printf("LEFT [%d] ",value);
	setMotorSpeed(MOTORG, value);
}

void    cmdRightMotor(int8_t value)
{
	printf("RIGHT [%d]\n\r",value);
	setMotorSpeed(MOTORD, value);
}

void    stopMotors(void)
{
	setMotorSpeed(MOTORG, 0);
	setMotorSpeed(MOTORD, 0);
}

void    readWriteWithValidation(uint8_t *buffer_read, uint8_t *buffer_send)
{
	HAL_SPI_Receive(&hspi2, buffer_read, 4, 250000);
	if (memcmp(buffer_read, CMD_R, 4) == 0)
		spiRead(buffer_read);
	else if (memcmp(buffer_read, CMD_W, 4) == 0)
		spiWrite(buffer_send);
	else
		spiError(buffer_read, buffer_send);

}

uint8_t blueButtonPressed(void)
{
	if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13))
	{
		ButtonPressed = ~(ButtonPressed) & 0x01;
		printf("pressed : %d\n\r", ButtonPressed);
		HAL_Delay(250);
		return(1);
	}
	return(0);
}

void    waitPressBlueButton(void)
{
	printf("wait press blue button\r\n");
	while (!blueButtonPressed())
		;
}

void    spiError(uint8_t *buffer_read, uint8_t *buffer_send)
{
	printf("ERROR: wait CMD_R || CMD_W [%d %d %d %d] from master\n\r", buffer_read[0], buffer_read[1], buffer_read[2], buffer_read[3]);
	HAL_SPI_Receive(&hspi2, buffer_read, 1, 250000);
	printf("DEBUG: Read(1): %d ", buffer_read[0]);
	while(buffer_read[0] != 0)
	{
		HAL_SPI_Receive(&hspi2, buffer_read, 1, 250000);
		printf(": %d ", buffer_read[0]);
	}
	spiWrite(buffer_send);
	printf("\n\r");
}
