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
#include "joystick.h"
#include "keyboard.h"
#include "network.h"
#include "opentyr.h"
#include "video.h"
#include "video_scale.h"

#include "SDL.h"
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

JE_boolean ESCPressed;

JE_boolean newkey, newmouse, keydown, mousedown;
SDLKey lastkey_sym;
SDLMod lastkey_mod;
unsigned char lastkey_char;
Uint8 lastmouse_but;
Uint16 lastmouse_x, lastmouse_y;
JE_boolean mouse_pressed[3] = {false, false, false};
Uint16 mouse_x, mouse_y;

Uint8 keysactive[SDLK_LAST];

#ifdef NDEBUG
bool input_grab_enabled = true;
#else
bool input_grab_enabled = false;
#endif

typedef struct {
	int gpio;
	SDL_Keycode key;
} GPIOKeyMap;

//Mappings from PS2 buttons to keys
static const GPIOKeyMap keymap[]={
	{36, SDLK_UP},
	{34, SDLK_DOWN},
	{32, SDLK_LEFT},
	{39, SDLK_RIGHT},

	{33, SDLK_ESCAPE},				//cross
	{35, SDLK_SPACE},			//circle
	{0, NULL},
};

static xQueueHandle gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}


void gpioTask(void *arg) {
    uint32_t io_num;
	int level;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
			for (int i=0; keymap[i].key!=NULL; i++)
				if(keymap[i].gpio == io_num)
				{
					level = gpio_get_level(io_num);
					keysactive[keymap[i].key] = level?false:true;
					if(level == 0)
						newkey = true;
					lastkey_sym = keymap[i].key;
					//lastkey_mod = ev.key.keysym.mod;
					//lastkey_char = ev.key.keysym.unicode;
					keydown = level?false:true;
				}
        }
    }
}

void inputInit()
{
	gpio_config_t io_conf;
    io_conf.pull_down_en = 0;
    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    //bit mask of the pins, use GPIO... here
	for (int i=0; keymap[i].key!=NULL; i++)
    	if(i==0)
			io_conf.pin_bit_mask = (1ULL<<keymap[i].gpio);
		else
			io_conf.pin_bit_mask |= (1ULL<<keymap[i].gpio);

	io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //start gpio task
	//xTaskCreatePinnedToCore(&gpioTask, "GPIO", 1500, NULL, 7, NULL, 0);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_SHARED);
    //hook isr handler for specific gpio pin
	for (int i=0; keymap[i].key!=NULL; i++)
    	gpio_isr_handler_add(keymap[i].gpio, gpio_isr_handler, (void*) keymap[i].gpio);

	//printf("keyboard: GPIO task created.\n");
}

void flush_events_buffer( void )
{
}

void wait_input( JE_boolean keyboard, JE_boolean mouse, JE_boolean joystick )
{
}

void wait_noinput( JE_boolean keyboard, JE_boolean mouse, JE_boolean joystick )
{
}

void init_keyboard( void )
{
	newkey = newmouse = false;
	keydown = mousedown = false;
	inputInit();
}

void input_grab( bool enable )
{

}

JE_word JE_mousePosition( JE_word *mouseX, JE_word *mouseY )
{
	*mouseX = mouse_x;
	*mouseY = mouse_y;
	return mousedown ? lastmouse_but : 0;
}

void set_mouse_position( int x, int y )
{
	if (input_grab_enabled)
	{
		mouse_x = x;
		mouse_y = y;
	}
}

void service_SDL_events( JE_boolean clear_new )
{

	if (clear_new)
		newkey = newmouse = false;
	uint32_t io_num;
	if(xQueueReceive(gpio_evt_queue, &io_num, 0)) {
		for (int i=0; keymap[i].key!=NULL; i++)
			if(keymap[i].gpio == io_num)
			{
				int level = gpio_get_level(io_num);
				keysactive[keymap[i].key] = level?false:true;
				if(level == 0)
					newkey = true;
				lastkey_sym = keymap[i].key;
				//lastkey_mod = ev.key.keysym.mod;
				//lastkey_char = ev.key.keysym.unicode;
				keydown = level?false:true;
			}
	}
	/*
	case SDL_QUIT:
		exit(0);
	*/

}

void JE_clearKeyboard( void )
{
	// /!\ Doesn't seems important. I think. D:
}

