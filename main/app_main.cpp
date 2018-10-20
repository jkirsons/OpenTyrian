#include <Arduino.h>
#include "opentyr.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>



void tyrianTask(void *pvParameters)
{
    char *argv[]={"opentyrian", NULL};
    main(1, argv);
}

void setup() {
    // put your setup code here, to run once:
    xTaskCreatePinnedToCore(&tyrianTask, "tyrianTask", 8096, NULL, 5, NULL, 0);
}

void loop() {
    // put your main code here, to run repeatedly:
}