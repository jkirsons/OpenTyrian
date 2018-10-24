
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
    char *argv[]={"opentyrian", NULL};
    main(1, argv);
}


//extern "C"
void app_main(void)
{
	xTaskCreatePinnedToCore(&tyrianTask, "tyrianTask", 28000, NULL, 5, NULL, 0);
}
