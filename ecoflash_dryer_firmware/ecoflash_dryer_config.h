
#ifndef ECOFLASH_DRYER_CONFIG_H
    #define ECOFLASH_DRYER_CONFIG_H

    #define DEBUG_SENSORS
    #define NB_OF_SENSORS           3  
    #define TEMP_THRESHOLD_LOW      40
    #define TEMP_THRESHOLD_HIGH     100
    #define HUMD_THRESHOLD_HIGH     25
    #define HUMD_THRESHOLD_LOW      10
    #define FEED_THRESHOLD_LOW      2
    #define FEED_THRESHOLD_HIGH     10
    #define TEST_RUN_PERIOD_SECS    10

    typedef enum PARAM_STATE
    {
        NORMAL_LEVEL,
        LOW_LEVEL,
        HIGH_LEVEL
    }
    PARAM_STATE;

    typedef enum ECOFLASH_STATES
    {
        IDLE_STATE,
        RUN_STATE,
        STOP_STATE,
        TEST_RUN_STATE,
        ERROR_STATE
    }ECOFLASH_STATES;

    typedef struct DEVICE_PARAMS
    {
        ECOFLASH_STATES ecoflash_state;
        uint8_t temperature;
        uint8_t humidity;
        uint8_t feeder;
        PARAM_STATE temp_state:2,
                    humd_state:2,
                    feed_state:2;
        bool motor_state:2;
    }
    DEVICE_PARAMS;

    extern DEVICE_PARAMS device_params;

    bool sensorInit( void );
    bool sensorUpdate( void );

    bool displayInit( void );
    void displayUpdate( void );
    void displayLog( String error );

    void commUpdate( void );
    bool commInit( void );
    
#endif