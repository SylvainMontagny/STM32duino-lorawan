/*---------------------------------------------------------------------------------------------------------------
 ACTIVATION_MODE     |    ABP or OTAA    |    # Selection of Activation Method                                  |
 SEND_BY_PUSH_BUTTON |   true or false   |    # Sending method (Time or Push Button)                            |  
 FRAME_DELAY         |    Time in ms     |    # Time between 2 frames (Minimum 7000)                            |
 SPREADING_FACTOR    |   Number [7;12]   |    # 7=SF7, 8=SF8, ..., 12=SF12                                      |
 ADAPTIVE_DR         |  true or false    |    # Enable Aaptive Data Rate (if true)                              |
 CONFIRMED           |  true or false    |    # Frame Confirmed (if true) OR Frame Unconfirmed (if false)       |
 PORT                |   Number [0;199]  |    # Application Port number                                         |
 ENABLE_HUMIDITY     |  true or false    |    # Enable Humidity Sensor (if true)                                |
 CAYENNE_LPP         |  true or false    |    # Enable all sensor and use the CAYENNE LPP format (if true)      |
 LOW_POWER           |  true or false    |    # Enable Low Power mode between two frames (if true)              |
---------------------------------------------------------------------------------------------------------------*/


#define ACTIVATION_MODE     ABP
#define SEND_BY_PUSH_BUTTON false
#define FRAME_DELAY         10000
#define SPREADING_FACTOR    7
#define ADAPTIVE_DR         false
#define CONFIRMED           false
#define PORT                1 
#define ENABLE_HUMIDITY     false
#define CAYENNE_LPP         false
#define LOW_POWER           false


// Configuration for ABP Activation Mode
const char devAddr[] = "";
const char nwkSKey[] = "";
const char appSKey[] = "";


// Configuration for OTAA Activation Mode
const char appKey[] = "";
const char appEUI[] = "0000000000000000";
