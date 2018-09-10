#include <Arduino.h>
#include "opentyr.h"

void tyrianTask(void *pvParameters)
{
    char *argv[]={"opentyrian", NULL};
    main(1, argv);
}

void setup() {
    // put your setup code here, to run once:
    xTaskCreatePinnedToCore(&tyrianTask, "tyrianTask", 22480, NULL, 5, NULL, 0);
}

void loop() {
    // put your main code here, to run repeatedly:
}