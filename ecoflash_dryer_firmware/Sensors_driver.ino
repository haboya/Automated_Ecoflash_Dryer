
#include "ecoflash_dryer_config.h"
#include <DallasTemperature.h>
#include <OneWire.h>

#define TEMP_SENSOR     8
#define HUMD_SENSOR       6
#define FEED_SENSOR_DT    10
#define FEED_SENSOR_SK      9

OneWire oneWire(TEMP_SENSOR);
DallasTemperature tempSensor(&oneWire);

const uint8_t MIN_SENSOR_VAL = 0;
const uint8_t MAX_SENSOR_VAL = 120;
const uint8_t SENSOR_SAMPLES = 10;

uint8_t sensor_read;

void readTemperature( void )
{
    tempSensor.requestTemperatures();

    float temp = tempSensor.getTempCByIndex(0);
    #ifdef DEBUG_SENSORS
        Serial.print(F("temp: "));
        Serial.println(temp);
    #endif

    if(temp > MIN_SENSOR_VAL && temp < MAX_SENSOR_VAL)
    {
        device_params.temperature = (uint8_t)temp;
    }
    else{
        device_params.temperature = MIN_SENSOR_VAL;
    }
}

void readHumidity( void )
{
    // uint16_t buffer_arr[SENSOR_SAMPLES], tempval;
    // for(byte i=0; i<SENSOR_SAMPLES; i++) 
    // { 
    //     buffer_arr[i] = analogRead(HUMD_SENSOR);
    //     delay(SENSOR_SAMPLES);
    // }

    // for(byte i=0;i<9;i++)
    // {
    //     for(int j=i+1;j<10;j++)
    //     {
    //         if(buffer_arr[i]>buffer_arr[j])
    //         {
    //             tempval = buffer_arr[i];
    //             buffer_arr[i] = buffer_arr[j];
    //             buffer_arr[j] = tempval;
    //         }
    //     }
    // }

    // unsigned long avgval = 0;
    // for(byte i=2; i<8; i++)
    //     avgval += buffer_arr[i];

    // float ph_act = (float)avgval*5.0/1024/6;
    // ph_act = -5.70 * ph_act + PH_FACTOR;
    // #ifdef DEBUG_SENSORS
    //     Serial.print(F("ph: "));
    //     Serial.println(ph_act);
    // #endif
    // if(ph_act > MIN_SENSOR_VAL && ph_act < MAX_SENSOR_VAL)
    // {
    //     device_params.humidity = (uint8_t)(ph_act*SENSOR_SAMPLES);
    // }
    // else
    // {
    //     device_params.humidity = MIN_SENSOR_VAL;
    // }
}

void readFeeder( void )
{
    // float volt = 0;
    // for(byte i=0; i<(SENSOR_SAMPLES*SENSOR_SAMPLES); i++)
    // {
    //     volt += ((float)analogRead(FEED_SENSOR)/1023)*5;
    // }

    // volt = volt/(SENSOR_SAMPLES*SENSOR_SAMPLES);
    // #ifdef DEBUG_SENSORS
    //     Serial.print(F("volt: "));
    //     Serial.println(volt);
    // #endif

    // if(volt > MIN_SENSOR_VAL)
    // {

    //     device_params.feeder = (uint8_t)((volt/3000.0*5.0)*10);
    //     #ifdef DEBUG_SENSORS
    //     Serial.print(F("NTU: "));
    //     Serial.println(device_params.feeder);
    //     #endif
    // }
}

bool sensorInit( void )
{
    tempSensor.begin();
    sensor_read = 0;

    return true;
}

bool sensorUpdate( void )
{
    switch (sensor_read)
    {
        case 0:
            readTemperature();
            if(device_params.temperature < TEMP_THRESHOLD_LOW)
            {
                device_params.temp_state = LOW_LEVEL;
            }
            else if(device_params.temperature > TEMP_THRESHOLD_HIGH)
            {
                device_params.temp_state = HIGH_LEVEL;
            }
            else
            {
                device_params.temp_state = NORMAL_LEVEL;
            }
        break;

        case 1:
            readHumidity();
            if(device_params.humidity < HUMD_THRESHOLD_LOW)
            {
                device_params.humd_state = LOW_LEVEL;
            }
            else if(device_params.humidity > HUMD_THRESHOLD_HIGH)
            {
                device_params.humd_state = HIGH_LEVEL;
            }
            else
            {
                device_params.humd_state = NORMAL_LEVEL;
            }
        break;

        case 2:
            readFeeder();
            if(device_params.feeder < FEED_THRESHOLD_LOW)
            {
                device_params.feed_state = LOW_LEVEL;
            }
            else if(device_params.feeder > FEED_THRESHOLD_HIGH)
            {
                device_params.feed_state = HIGH_LEVEL;
            }
            else
            {
                device_params.feed_state = NORMAL_LEVEL;
            }
        break;
        
        default:
        break;
    }

    sensor_read = (++sensor_read)%NB_OF_SENSORS;

    return true;
}

/* ___________________________________ EOF __________________________________ */