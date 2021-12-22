/*---------------------------------------------------------------------------------------------------------------
 ACTIVATION_MODE     |    ABP or OTAA    |    # Selection of Activation Method                                  |
 SEND_BY_PUSH_BUTTON |   true or false   |    # Sending method (Time or Push Button)                            |  
 FRAME_DELAY         |    Time in ms     |    # Time between 2 frames (Minimum 7000)                            |
 DATA_RATE           |   Number [0;5]    |    # 5=SF7BW125 ->  0=SF12BW125                                      |
 ADAPTIVE_DR         |  true or false    |    # Enable Aaptive Data Rate (if true)                              |
 CONFIRMED           |  true or false    |    # Frame Confirmed (if true) OR Frame Unconfirmed (if false)       |
 PORT                |   Number [0;199]  |    # Application Port number                                         |
 ENABLE_HUMIDITY     |  true or false    |    # Enable Humidity Sensor (if true)                                |
 CAYENNE_LPP         |  true or false    |    # Enable all sensor and use the CAYENNE LPP format (if true)      |
 LOW_POWER           |  true or false    |    # Enable Low Power mode between two frames (if true)              |
---------------------------------------------------------------------------------------------------------------*/


#define ACTIVATION_MODE     OTAA
#define SEND_BY_PUSH_BUTTON true
#define FRAME_DELAY         10000
#define DATA_RATE           5
#define ADAPTIVE_DR         false
#define CONFIRMED           false
#define PORT                2 
#define ENABLE_HUMIDITY     false
#define CAYENNE_LPP         false
#define LOW_POWER           false


// Configuration for ABP Activation Mode
const char devAddr[] = "0198F02F";
const char nwkSKey[] = "6FE8DBC22F848A16EE7E29D73AF9492C";
const char appSKey[] = "51360BCB7E7926A127B97A9AD19A4C61";


// Configuration for OTAA Activation Mode
const char appKey[] = "BFFAEFC231CA5697CF9B2AE413C48C46";
const char appEUI[] = "0000000000000000";
