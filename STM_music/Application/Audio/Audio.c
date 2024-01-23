#include "Audio.h"
#include "stm32f4xx.h"

#include <stdlib.h>
#include "play_i2s.h"
#include "main.h"

static void WriteRegister(uint8_t address, uint8_t value);
static void StopAudioDMA();

static AudioCallbackFunction *CallbackFunction;

void InitializeAudio(int plln, int pllr, int i2sdiv, int i2sodd) {
//	GPIO_InitTypeDef  GPIO_InitStructure;

	// Intitialize state.
	CallbackFunction = NULL;

	cs43l22_init();

	cs43l22_write(0x0a, 0x00); // Disable the analog soft ramp.
	cs43l22_write(0x0e, 0x04); // Disable the digital soft ramp.

	cs43l22_write(0x27, 0x00); // Disable the limiter attack level.
	cs43l22_write(0x1f, 0x0f); // Adjust bass and treble levels.

	cs43l22_write(0x1a, 0x0a); // Adjust PCM volume level.
	cs43l22_write(0x1b, 0x0a);
}

void AudioOn() {
	WriteRegister(0x02, 0x9e);
//	SPI3 ->I2SCFGR = SPI_I2SCFGR_I2SMOD | SPI_I2SCFGR_I2SCFG_1
//			| SPI_I2SCFGR_I2SE; // Master transmitter, Phillips mode, 16 bit values, clock polarity low, enable.

}

void AudioOff() {
	WriteRegister(0x02, 0x01);
//	SPI3 ->I2SCFGR = 0;
}

void SetAudioVolume(int volume) {
//	WriteRegister(0x20, (volume + 0x19) & 0xff);
//	WriteRegister(0x21, (volume + 0x19) & 0xff);
	cs43l22_write(0x20, (volume + 0x19) & 0xff);
	cs43l22_write(0x21, (volume + 0x19) & 0xff);
}

void OutputAudioSample(int16_t sample) {
//	while (!(SPI3 ->SR & SPI_SR_TXE ))
//		;
//	SPI3 ->DR = sample;
}

void OutputAudioSampleWithoutBlocking(int16_t sample) {
//	SPI3 ->DR = sample;
}

void StopAudio() {
//	StopAudioDMA();
//	SPI1 ->CR2 &= ~SPI_CR2_TXDMAEN; // Disable I2S TX DMA request.
//	NVIC_DisableIRQ(DMA1_Stream5_IRQn);
//	CallbackFunction = NULL;
}

void PlayAudioWithCallback(AudioCallbackFunction *callback) {
	CallbackFunction = callback;
}

void ProvideAudioBuffer(void *samples, int numsamples) {

	HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t*)samples, numsamples);
}

static void WriteRegister(uint8_t address, uint8_t value) {
}

static void StopAudioDMA() {
//	DMA1_Stream5 ->CR &= ~DMA_SxCR_EN; // Disable DMA stream.
//	while (DMA1_Stream5 ->CR & DMA_SxCR_EN); // Wait for DMA stream to stop.
}



void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{

}


void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{

	if (CallbackFunction)
			CallbackFunction();
}



