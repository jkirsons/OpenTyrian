#include "SDL_audio.h"

SDL_AudioSpec as;
unsigned char *sdl_buffer;
void *user_data;
bool paused = true;
bool locked = false;
xSemaphoreHandle xSemaphoreAudio = NULL;

IRAM_ATTR void updateTask(void *arg)
{
  size_t bytesWritten;
  while(1)
  {
	  if(!paused && /*xSemaphoreAudio != NULL*/ !locked && sdl_buffer){
			// clear buffer
		  //xSemaphoreTake( xSemaphoreAudio, portMAX_DELAY );
		  memset(sdl_buffer, 0, SAMPLECOUNT*SAMPLESIZE);
		  (*as.callback)(NULL, sdl_buffer, SAMPLECOUNT*SAMPLESIZE );
		  ESP_ERROR_CHECK(i2s_write(I2S_NUM_0, sdl_buffer, SAMPLECOUNT*SAMPLESIZE, &bytesWritten, 50 / portTICK_PERIOD_MS));
		  //xSemaphoreGive( xSemaphoreAudio );
		  //vTaskDelay( 1 );
	  } else
		  vTaskDelay( 5 );
  }
}

int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained)
{
	static const i2s_config_t i2s_config = {
	.mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN,
	.sample_rate = SAMPLERATE,
	.bits_per_sample = SAMPLESIZE*8, /* the DAC module will only take the 8bits from MSB */
	.channel_format = I2S_CHANNEL_FMT_ALL_LEFT,
	.communication_format = I2S_COMM_FORMAT_I2S_MSB,
	.intr_alloc_flags = 0,
	.dma_buf_count = 4,
	.dma_buf_len = 512,
	.use_apll = false
	};
	static const int i2s_num = I2S_NUM_0; // i2s port number

	ESP_ERROR_CHECK(i2s_driver_install(i2s_num, &i2s_config, 0, NULL));   //install and start i2s driver
	i2s_pin_config_t pc = {
        .bck_io_num = 26,
        .ws_io_num = -1,
        .data_out_num = -1,
        .data_in_num = -1
    };
	ESP_ERROR_CHECK(i2s_set_pin(i2s_num, &pc));

	//ESP_ERROR_CHECK(i2s_set_pin(i2s_num, NULL));
	ESP_ERROR_CHECK(i2s_set_clk(i2s_num, SAMPLERATE, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_STEREO));
	//ESP_ERROR_CHECK(i2s_set_dac_mode(I2S_DAC_CHANNEL_LEFT_EN));
	//ESP_ERROR_CHECK(i2s_set_sample_rates(i2s_num, SAMPLERATE)); 

	sdl_buffer = malloc(SAMPLECOUNT * SAMPLESIZE * 2);

	memset(obtained, 0, sizeof(SDL_AudioSpec)); 
	obtained->freq = SAMPLERATE;
	obtained->format = 16;
	obtained->channels = 1;
	obtained->samples = SAMPLECOUNT;
	obtained->callback = desired->callback;
	memcpy(&as,obtained,sizeof(SDL_AudioSpec));

	dac_output_enable(DAC_CHANNEL_1);
	dac_output_voltage(DAC_CHANNEL_1, 255);    

	//xSemaphoreAudio = xSemaphoreCreateBinary();
	xTaskCreatePinnedToCore(&updateTask, "updateTask", 6000, NULL, 3, NULL, 1);
	printf("audio task started\n");
	return 0;
}

void SDL_PauseAudio(int pause_on)
{
	paused = pause_on;
}

void SDL_CloseAudio(void)
{
	  i2s_driver_uninstall(I2S_NUM_0); //stop & destroy i2s driver
	  free(sdl_buffer);
}

int SDL_BuildAudioCVT(SDL_AudioCVT *cvt, Uint16 src_format, Uint8 src_channels, int src_rate, Uint16 dst_format, Uint8 dst_channels, int dst_rate)
{
	cvt->len_mult = 1;
	return 0;
}

int SDL_ConvertAudio(SDL_AudioCVT *cvt)
{
	Sint16 *sbuf = cvt->buf;
	Uint8 *ubuf = cvt->buf;

	for(int i = 0; i < cvt->len*2; i+=2)
	{
		ubuf[i] =   ((Sint32)sbuf[i/2] + (Sint32)0x8000) >> 8 ;
		ubuf[i+1] = ((Sint32)sbuf[i/2] + (Sint32)0x8000) >> 8;

	}
	/*
		for(int i = 0; i < cvt->len/2; i++)
	{
		sbuf[i*2] = (i <  cvt->len/4) * 0x8000;
		sbuf[i*2+1] = 0;
	}
	*/
	return 0;
}

void SDL_LockAudio(void)
{
	locked = true;
	//if( xSemaphoreAudio != NULL )
	//	xSemaphoreTake( xSemaphoreAudio, 100 );
}

void SDL_UnlockAudio(void)
{
    locked = false;
	//if( xSemaphoreAudio != NULL )
	//	 xSemaphoreGive( xSemaphoreAudio );
}

