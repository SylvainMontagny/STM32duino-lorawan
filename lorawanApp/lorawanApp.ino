#include "LoRaWANNode.h"
#include "STM32LowPower.h"
#include "config.h"


char frameTx[] = "Hello";
String str;

HardwareSerial SerialLora(D0, D1); // D0(Rx) D1(TX)
HardwareSerial Serial1(PA10, PA9);

#if ACTIVATION_MODE == ABP
const char devAddr[] = "0198F02F";
const char nwkSKey[] = "6FE8DBC22F848A16EE7E29D73AF9492C";
const char appSKey[] = "51360BCB7E7926A127B97A9AD19A4C61";
#endif

#if ACTIVATION_MODE == OTAA
const char appKey[] = "";
const char appEUI[] = "";
#endif


void setup()
{  
  Serial1.begin(115200);
#if LOW_POWER == true
  LowPower.begin();
#endif
  pinMode(PUSHBUTTON, INPUT);
  infoBeforeActivation();
  activation();
  infoAfterActivation();
}

void loop()
{
  if( SEND_BY_PUSH_BUTTON == 1)   while(digitalRead(PUSHBUTTON)); // Attente Push Button pour envoyer
  else                            delay((FRAME_DELAY<7000)?0:FRAME_DELAY-7000);  // Attente FRAME_DELAY pour envoyer
  Serial1.print(" Sending Text : \"");Serial1.print(frameTx);Serial1.print("\"");
  if(CONFIRMED)   Serial1.print(" Uplink CONFIRMED on PORT ");
  else            Serial1.print(" Uplink UNCONFIRMED on PORT ");
  Serial1.println(PORT);
  transmit();
  receive();
  
#if LOW_POWER == true
  Serial1.print(" Processor and LoRaWAN module go to sleep during ");
  Serial1.print(FRAME_DELAY);
  Serial1.println(" ms");
  delay(500);
  loraNode.sleep();
  LowPower.deepSleep(FRAME_DELAY);
  loraNode.wakeup();
  Serial1.println(" Processor and LoRaWAN module wake up !!!!\r\n");
#endif

}

void receive(void) {
  uint8_t frameRx[64];
  uint8_t len;
  uint8_t port;

  if(loraNode.receiveFrame(frameRx, &len, &port)) {
    uint8_t n = 0;
    Serial1.print(" Frame received: ");
    while(len > 0) {
      Serial1.print(frameRx[n], HEX);
      Serial1.print("(Hexa) ");
      Serial1.print(frameRx[n]);
      Serial1.print("(Dec), ");
      len--;
      n++;
    }
    Serial1.print(" on port "); Serial1.println(port);Serial1.print("\r\n");
  } else {
    Serial1.println(" No data received\r\n\r\n");
  }
}

void transmit(void) {
  
  int status = loraNode.sendFrame(frameTx, sizeof(frameTx), CONFIRMED, PORT);
  if(status == LORA_SEND_ERROR) {
    Serial1.println(" Send frame failed!!!");
  } else if(status == LORA_SEND_DELAYED) {
    Serial1.println(" Module is busy : \r\n * It's still trying to send data \r\n OR * \r\n * You are over your allowed duty cycle");
  } else {
    Serial1.println(" Frame sent. Waiting for Downlink...");
  }
}


void infoAfterActivation(void){
   if(SEND_BY_PUSH_BUTTON == 0){
    Serial1.print(" Frame will be sent every ");Serial1.print((FRAME_DELAY<7000)?7000:FRAME_DELAY);Serial1.println("ms\r\n");
  }
  else {
    Serial1.println(" Press Blue Button to send a Frame\r\n");
  }
  
}

/***************************************************************/
/******************* ABP Function ******************************/
/***************************************************************/
#if ACTIVATION_MODE == ABP
void infoBeforeActivation(void){
  Serial1.println("\r\n\r\n\r\n");
  Serial1.println("#######################################");
  Serial1.println("######## Formation LoRaWAN ############");
  Serial1.println("######### ABP activation  ##########\r\n");

  // Enable the USI module and set the radio band.
  while(!loraNode.begin(&SerialLora, LORA_BAND_EU_868)) {
    Serial1.println("Lora module not ready");
    delay(1000);
  }
  loraNode.setDutyCycle(false); 
  Serial1.print(" * Device Address :          0x ");
  Serial1.println(devAddr);
  Serial1.print(" * Network Session Key :     0x ");
  Serial1.println(nwkSKey);
  Serial1.print(" * Application Session Key : 0x ");
  Serial1.println(appSKey);Serial1.print("\r\n");
  
 loraNode.setAdaptativeDataRate(DISABLE);
 loraNode.setDataRate(DATA_RATE);
 Serial1.print(" * Data Rate : ");Serial1.print(loraNode.getDataRate());Serial1.print("\r\n");
 if(ADAPTIVE_DR) {
    loraNode.setAdaptativeDataRate(ENABLE);
    Serial1.print(" * Adaptive Data Rate : ON");Serial1.println("\r\n");
  }
  else {
    Serial1.print(" * Adaptive Data Rate : OFF");Serial1.println("\r\n");
  }       
  loraNode.setDutyCycle(DISABLE);
}


void activation(void){
  while(!loraNode.joinABP(devAddr, nwkSKey, appSKey)) {
    Serial1.println(" The Device is not activated on the Server!!");
    delay(1000);
  }
}
#endif

/***************************************************************/
/******************* OTAA Function *****************************/
/***************************************************************/

#if ACTIVATION_MODE == OTAA
void infoBeforeActivation(void){
  Serial1.println("\r\n\r\n\r\n");
  Serial1.println("########################################");
  Serial1.println("######## LoRaWAN Training Session ######");
  Serial1.println("#########     OTAA activation   ########\r\n");

  while(!loraNode.begin(&SerialLora, LORA_BAND_EU_868)) {
    Serial1.println(" Lora module not ready");
    delay(1000);
  }
 
  str = " * Device EUI :      0x ";
  loraNode.getDevEUI(&str);
  Serial1.println(str);
  Serial1.print(" * Application key : 0x ");
  Serial1.println(appKey);
  Serial1.print(" * Application EUI : 0x ");
  Serial1.println(appEUI);Serial1.print("\r\n");

  loraNode.setAdaptativeDataRate(DISABLE);
  loraNode.setDataRate(DATA_RATE);
  Serial1.print(" * Data Rate : ");Serial1.print(loraNode.getDataRate());Serial1.print("\r\n");
  if(ADAPTIVE_DR) {
    loraNode.setAdaptativeDataRate(ENABLE);
    Serial1.print(" * Adaptive Data Rate : ON");Serial1.println("\r\n");
  }
  else {
    Serial1.print(" * Adaptive Data Rate : OFF");Serial1.println("\r\n");
  }       
  loraNode.setDutyCycle(DISABLE);
}


void activation(void){
  Serial1.println(" JOIN procedure in progress ...");  
  
  while(!loraNode.joinOTAA(appKey, appEUI)) {
    Serial1.println(" JOIN OTAA failed!!! Retry...");
    delay(1000);
  }
  Serial1.println(" JOIN procedure : SUCCESS !\r\n");
}
#endif
