// I2S
#include "play_i2s.h"
#define AUDIO_I2C_ADDR	0x94

void cs43l22_write(uint8_t reg, uint8_t value)
{
	HAL_I2C_Mem_Write(&hi2c1, AUDIO_I2C_ADDR, reg, 1, &value, sizeof(value), HAL_MAX_DELAY);
}

void cs43l22_init(void)
{
	HAL_GPIO_WritePin(AUDIO_RST_GPIO_Port, AUDIO_RST_Pin, GPIO_PIN_SET);

	cs43l22_write(0x04, 0xaf);
	cs43l22_write(0x06, 0x07);
	cs43l22_write(0x02, 0x9e);
}