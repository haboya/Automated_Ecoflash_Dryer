
#include "ecoflash_dryer_config.h"
#include <SoftwareSerial.h>

SoftwareSerial webSerial(3,2);

unsigned long previous_send;

void commUpdate( void )
{
    if(millis() - previous_send > 500)
    {
        String curr_status = "*";
        curr_status += String(device_params.motor_state) + ",";
        curr_status += String(device_params.temperature) + ",";
        curr_status += String(device_params.humidity) + "#";
        webSerial.println(curr_status);

        previous_send = millis();
    }
}

bool commInit( void )
{
    webSerial.begin(9600);
    previous_send = millis();

    return true;
}
