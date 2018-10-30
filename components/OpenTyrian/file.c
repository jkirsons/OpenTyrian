/* 
 * OpenTyrian: A modern cross-platform port of Tyrian
 * Copyright (C) 2007-2009  The OpenTyrian Development Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "file.h"
#include "opentyr.h"
#include "varz.h"

#include "SDL.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"

#define MODE_SPI 1
#define PIN_NUM_MISO CONFIG_HW_SD_PIN_NUM_MISO
#define PIN_NUM_MOSI CONFIG_HW_SD_PIN_NUM_MOSI
#define PIN_NUM_CLK  CONFIG_HW_SD_PIN_NUM_CLK
#define PIN_NUM_CS   CONFIG_HW_SD_PIN_NUM_CS

const char *custom_data_dir = NULL;

static bool init_SD = false;

void Init_SD()
{
#if MODE_SPI == 1
	sdmmc_host_t host = SDSPI_HOST_DEFAULT();
	//host.command_timeout_ms=200;
	//host.max_freq_khz = SDMMC_FREQ_PROBING;
    sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
    slot_config.gpio_miso = PIN_NUM_MISO;
    slot_config.gpio_mosi = PIN_NUM_MOSI;
    slot_config.gpio_sck  = PIN_NUM_CLK;
    slot_config.gpio_cs   = PIN_NUM_CS;
	slot_config.dma_channel = 1; //2
#else
	sdmmc_host_t host = SDMMC_HOST_DEFAULT();
	host.flags = SDMMC_HOST_FLAG_1BIT;
	//host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
	host.command_timeout_ms=500;
	sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
	slot_config.width = 1;
#endif
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 10
    };

	sdmmc_card_t* card;
    esp_err_t ret = esp_vfs_fat_sdmmc_mount("", &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
        	fprintf(stderr, "Init_SD: Failed to mount filesystem.\n");
        } else {
        	fprintf(stderr, "Init_SD: Failed to initialize the card. %d\n", ret);
        }
        return;
    }
    fprintf(stderr, "Init_SD: SD card opened.\n");
	//sdmmc_card_print_info(stdout, card);
	init_SD = true;
}

// finds the Tyrian data directory
const char *data_dir( void )
{
	const char *dirs[] =
	{
		custom_data_dir,
		TYRIAN_DIR,
		"data",
		".",
	};
	
	static const char *dir = NULL;
	
	if (dir != NULL)
		return dir;
	
	for (uint i = 0; i < COUNTOF(dirs); ++i)
	{
		if (dirs[i] == NULL)
			continue;
		
		FILE *f = dir_fopen(dirs[i], "tyrian1.lvl", "rb");
		if (f)
		{
			fclose(f);
			
			dir = dirs[i];
			break;
		}
	}
	
	if (dir == NULL) // data not found
		dir = "";
	
	return dir;
}

// prepend directory and fopen
FILE *dir_fopen( const char *dir, const char *file, const char *mode )
{
	fprintf(stderr, "Opening File: %s\n", file);
	if(init_SD == false)
		Init_SD();

	char *path = (char *)malloc(strlen(dir) + 1 + strlen(file) + 1);
	sprintf(path, "%s/%s", dir, file);
	
	FILE *f = fopen(path, mode);
	
	free(path);
	
	return f;
}

// warn when dir_fopen fails
FILE *dir_fopen_warn(  const char *dir, const char *file, const char *mode )
{
	FILE *f = dir_fopen(dir, file, mode);
	
	if (f == NULL)
		fprintf(stderr, "warning: failed to open '%s': %s\n", file, strerror(errno));
	
	return f;
}

// die when dir_fopen fails
FILE *dir_fopen_die( const char *dir, const char *file, const char *mode )
{
	FILE *f = dir_fopen(dir, file, mode);
	
	if (f == NULL)
	{
		fprintf(stderr, "error: failed to open '%s': %s\n", file, strerror(errno));
		fprintf(stderr, "error: One or more of the required Tyrian " TYRIAN_VERSION " data files could not be found.\n"
		                "       Please read the README file.\n");
		JE_tyrianHalt(1);
	}
	
	return f;
}

// check if file can be opened for reading
bool dir_file_exists( const char *dir, const char *file )
{
	FILE *f = dir_fopen(dir, file, "rb");
	if (f != NULL)
		fclose(f);
	return (f != NULL);
}

// returns end-of-file position
long ftell_eof( FILE *f )
{
	long pos = ftell(f);
	
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	
	fseek(f, pos, SEEK_SET);
	
	return size;
}

// endian-swapping fread that dies if the expected amount cannot be read
size_t efread( void *buffer, size_t size, size_t num, FILE *stream )
{
	size_t num_read = fread(buffer, size, num, stream);
	
	switch (size)
	{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		case 2:
			for (size_t i = 0; i < num; i++)
				((Uint16 *)buffer)[i] = SDL_Swap16(((Uint16 *)buffer)[i]);
			break;
		case 4:
			for (size_t i = 0; i < num; i++)
				((Uint32 *)buffer)[i] = SDL_Swap32(((Uint32 *)buffer)[i]);
			break;
		case 8:
			for (size_t i = 0; i < num; i++)
				((Uint64 *)buffer)[i] = SDL_Swap64(((Uint64 *)buffer)[i]);
			break;
#endif
		default:
			break;
	}
	
	if (num_read != num)
	{
		fprintf(stderr, "error: An unexpected problem occurred while reading from a file.\n");
		fprintf(stderr, "read bytes: %d, expected: %d\n", num_read, num);
		JE_tyrianHalt(1);
	}

	return num_read;
}

// endian-swapping fwrite that dies if the expected amount cannot be written
size_t efwrite( const void *buffer, size_t size, size_t num, FILE *stream )
{
	void *swap_buffer = NULL;
	
	switch (size)
	{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		case 2:
			swap_buffer = malloc(size * num);
			for (size_t i = 0; i < num; i++)
				((Uint16 *)swap_buffer)[i] = SDL_SwapLE16(((Uint16 *)buffer)[i]);
			buffer = swap_buffer;
			break;
		case 4:
			swap_buffer = malloc(size * num);
			for (size_t i = 0; i < num; i++)
				((Uint32 *)swap_buffer)[i] = SDL_SwapLE32(((Uint32 *)buffer)[i]);
			buffer = swap_buffer;
			break;
		case 8:
			swap_buffer = malloc(size * num);
			for (size_t i = 0; i < num; i++)
				((Uint64 *)swap_buffer)[i] = SDL_SwapLE64(((Uint64 *)buffer)[i]);
			buffer = swap_buffer;
			break;
#endif
		default:
			break;
	}
	
	size_t num_written = fwrite(buffer, size, num, stream);
	
	if (swap_buffer != NULL)
		free(swap_buffer);
	
	if (num_written != num)
	{
		fprintf(stderr, "error: An unexpected problem occurred while writing to a file.\n");
		JE_tyrianHalt(1);
	}
	
	return num_written;
}
