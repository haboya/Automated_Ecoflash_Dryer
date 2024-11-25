
#include "ecoflash_dryer_config.h"

#define START_BUTTON  4
#define STOP_BUTTON   5
#define BUZZER         11
#define MOTOR_REL       13
#define FEEDER_RED      A0
#define FEEDER_GREEN    A1
#define FEEDER_BLUE     A2
#define BUCKET_RED      A3
#define BUCKET_GREEN    A4
#define BUCKET_BLUE     A5
#define LED_OFF         1
#define LED_ON          0
#define MOTOR_OFF       0
#define MOTOR_ON        1
#define BUTTON_PRESSED  1
#define BUTTON_RELEASED 0

typedef enum LED_COLOR{
    NO_COLOR,
    RED_COLOR,
    GREEN_COLOR,
    BLUE_COLOR
}LED_COLOR;

DEVICE_PARAMS device_params;
unsigned long test_run_millis;

bool readButton(uint8_t btn_pin)
{
    if(digitalRead(btn_pin) == HIGH)
    {
        return HIGH;
    }

    return LOW;
}

void switchMotor(bool new_state)
{
    digitalWrite(MOTOR_REL, new_state);
    if(new_state == MOTOR_ON)
    {
        device_params.motor_state = true;
        Serial.println("Motor Started");
    }
    else 
    {
        device_params.motor_state = false;
        Serial.println("Motor Stopped");
    }
}

/// @brief Turn on the specified LED
/// @param feeder set to true if you intend to turn on the LED on the feeder, and false for the bucket
/// @param new_color the exact color the LED should be turned on
void switchOnLED( bool feeder, LED_COLOR new_color)
{
    if(feeder)
    {
        digitalWrite(FEEDER_RED, LED_OFF);
        digitalWrite(FEEDER_GREEN, LED_OFF);
        digitalWrite(FEEDER_BLUE, LED_OFF);

        if(new_color != NO_COLOR)
        {
            switch (new_color)
            {
                case RED_COLOR:
                    digitalWrite(FEEDER_RED, LED_ON);
                break;
                case GREEN_COLOR:
                    digitalWrite(FEEDER_GREEN, LED_ON);
                break;
                case BLUE_COLOR:
                    digitalWrite(FEEDER_BLUE, LED_ON);
                break;
            
            default:
                break;
            }
        }
    }
    else
    {
        digitalWrite(BUCKET_RED, LED_OFF);
        digitalWrite(BUCKET_GREEN, LED_OFF);
        digitalWrite(BUCKET_BLUE, LED_OFF);

        if(new_color != NO_COLOR)
        {
            switch (new_color)
            {
                case RED_COLOR:
                    digitalWrite(BUCKET_RED, LED_ON);
                break;
                case GREEN_COLOR:
                    digitalWrite(BUCKET_GREEN, LED_ON);
                break;
                case BLUE_COLOR:
                    digitalWrite(BUCKET_BLUE, LED_ON);
                break;
            
            default:
                break;
            }
        }
    }
    
}

void soundBuzzer( void )
{
    static unsigned long buzzer_millis = 0;
    if(millis() - buzzer_millis > 1500)
    {
        tone(BUZZER, 4000, 100);
        buzzer_millis = millis();
    }
}

void setup()
{
    Serial.begin(9600);
    pinMode(START_BUTTON, INPUT_PULLUP);
    pinMode(STOP_BUTTON, INPUT_PULLUP);
    pinMode(MOTOR_REL, OUTPUT);
    pinMode(BUZZER, OUTPUT);
    pinMode(FEEDER_RED, OUTPUT);
    pinMode(FEEDER_BLUE, OUTPUT);
    pinMode(FEEDER_GREEN, OUTPUT);
    pinMode(BUCKET_RED, OUTPUT);
    pinMode(BUCKET_BLUE, OUTPUT);
    pinMode(BUCKET_GREEN, OUTPUT);

    switchOnLED(true, BLUE_COLOR);
    switchOnLED(false, BLUE_COLOR);
    switchMotor(MOTOR_OFF);

    device_params.ecoflash_state = IDLE_STATE;

    if(!sensorInit())
    {
        switchOnLED(true, RED_COLOR);
        switchOnLED(false, RED_COLOR);
        device_params.ecoflash_state = ERROR_STATE;
    }

    if(!commInit())
    {
        switchOnLED(true, RED_COLOR);
        switchOnLED(false, RED_COLOR);
        device_params.ecoflash_state = ERROR_STATE;
    }

    delay(1000); //wait a bit for sensors to stabilize

}

void loop()
{
    sensorUpdate();

    if(device_params.temp_state == NORMAL_LEVEL)
    {
        switchMotor(MOTOR_ON);
    }

    else
    {
        switchMotor(MOTOR_OFF);
    }

    commUpdate();
}

/*
void loop()
{
    sensorUpdate();
    if((device_params.temp_state == HIGH_LEVEL) ||
        (device_params.humd_state == HIGH_LEVEL) ||
        (device_params.feed_state == HIGH_LEVEL)
    )
    {
        device_params.ecoflash_state = ERROR_STATE;
    }

    switch(device_params.ecoflash_state)
    {
        case IDLE_STATE:
            switchOnLED(false, GREEN_COLOR);
            switchMotor(MOTOR_OFF);

            if(readButton(START_BUTTON) == BUTTON_PRESSED)
            {
                device_params.ecoflash_state = TEST_RUN_STATE;
                test_run_millis = millis();
            }
            else if((device_params.temp_state == NORMAL_LEVEL) &&
                    (device_params.feed_state == NORMAL_LEVEL)
            )
            {
                device_params.ecoflash_state = RUN_STATE;
            }
            else if((device_params.temp_state == NORMAL_LEVEL) ||
                    (device_params.feed_state == NORMAL_LEVEL)
            )
            {
                soundBuzzer();
                switchOnLED(true, RED_COLOR);
            }
            else
            {
                switchOnLED(true, GREEN_COLOR);
            }
        break;

        case RUN_STATE:
            switchOnLED(true, BLUE_COLOR);
            switchMotor(MOTOR_ON);
            if((readButton(STOP_BUTTON) == BUTTON_PRESSED) ||
                (device_params.feed_state == LOW_LEVEL)
            )
            {
                device_params.ecoflash_state = STOP_STATE;
            }
            else if(device_params.humd_state != LOW_LEVEL)
            {
                soundBuzzer();
                switchOnLED(false, RED_COLOR);
            }
            else
            {
                switchOnLED(false, BLUE_COLOR);
            }

        break;

        case TEST_RUN_STATE:
            switchOnLED(true, BLUE_COLOR);
            switchOnLED(false, BLUE_COLOR);
            switchMotor(MOTOR_ON);

            if((millis() - test_run_millis > TEST_RUN_PERIOD_SECS*1000) ||
                (readButton(STOP_BUTTON) == BUTTON_PRESSED)
            )
            {
                device_params.ecoflash_state = STOP_STATE;
            }
        break;

        case STOP_STATE:
            switchOnLED(true, BLUE_COLOR);
            switchOnLED(false, BLUE_COLOR);
            switchMotor(MOTOR_OFF);

            device_params.ecoflash_state = IDLE_STATE;
        break;

        case ERROR_STATE:
            switchOnLED(true, RED_COLOR);
            switchOnLED(false, RED_COLOR);
            switchMotor(MOTOR_OFF);

            if(readButton(STOP_BUTTON) == BUTTON_PRESSED)
            {
                if(
                    (device_params.temp_state != HIGH_LEVEL) &&
                    (device_params.humd_state != HIGH_LEVEL) &&
                    (device_params.feed_state != HIGH_LEVEL)
                )
                {
                    device_params.ecoflash_state = IDLE_STATE;
                }
                else
                {
                    soundBuzzer();
                }
            }
        break;
    }

    commUpdate();
}
*/
