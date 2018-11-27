
#include "opentyr.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


void tyrianTask(void *pvParameters)
{
//    heap_caps_print_heap_info(MALLOC_CAP_SPIRAM);
//    spi_lcd_init();

    char *argv[]={"opentyrian", NULL};
    main(1, argv);
}


//extern "C"
void app_main(void)
{
	xTaskCreatePinnedToCore(&tyrianTask, "tyrianTask", 34000, NULL, 5, NULL, 0);
}
