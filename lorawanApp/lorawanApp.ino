#include "LoRaWANNode.h"
#include "STM32LowPower.h"
#include "config.h"

#if ENABLE_HUMIDITY == true || CAYENNE_LPP == true
      #include <HTS221Sensor.h>
      #include <LPS22HBSensor.h>
      #include <LSM303AGR_ACC_Sensor.h>
      #include <LSM303AGR_MAG_Sensor.h>
      #include <CayenneLPP.h>
      #define I2C2_SCL    D15//PB10
      #define I2C2_SDA    D14//PB11
      TwoWire *dev_i2c;
      HTS221Sensor  *HumTemp;
      LPS22HBSensor *PressTemp;
      LSM303AGR_ACC_Sensor *Acc;
      LSM303AGR_MAG_Sensor *Mag;
      // Read humidity, pressure, temperature from LPS22HB, temperature from HTS221
      float pressure, humidity, temperatureFromHTS221, temperatureFromLPS22HB, temperatureFromLSM303AGR;
      int32_t accelerometer[3];
      int32_t magnetometer[3];
      int8_t humidityLora =0;
      CayenneLPP lpp(51);
#endif


HardwareSerial SerialLora(D0, D1); // D0(Rx) D1(TX)
HardwareSerial Serial1(PA10, PA9);

char frameTx[] = "Hello";
String str;


void setup()
{  
  Serial1.begin(115200);
#if LOW_POWER == true
    LowPower.begin();
#endif

#if ENABLE_HUMIDITY == true || CAYENNE_LPP == true
      lpp.reset();  // Initialise CAYENNE 
      dev_i2c = new TwoWire(I2C2_SDA, I2C2_SCL);
      dev_i2c->begin();
      HumTemp = new HTS221Sensor (dev_i2c);
      HumTemp->Enable();
      PressTemp = new LPS22HBSensor(dev_i2c);
      PressTemp->Enable();
      Acc = new LSM303AGR_ACC_Sensor(dev_i2c);
      Acc->Enable();
      Acc->EnableTemperatureSensor();
      Mag = new LSM303AGR_MAG_Sensor(dev_i2c);
      Mag->Enable();
#endif
  
  pinMode(PUSHBUTTON, INPUT);
  showConfig();
  infoBeforeActivation();
  if (ACTIVATION_MODE == ABP) activationABP();
  if (ACTIVATION_MODE == OTAA) activationOTAA();
  infoAfterActivation();
}

void loop()
{
  if( SEND_BY_PUSH_BUTTON == 1)   while(digitalRead(PUSHBUTTON)); // Attente Push Button pour envoyer
  else                            delay((FRAME_DELAY<7000)?0:FRAME_DELAY-7000);  // Attente FRAME_DELAY pour envoyer

#if ENABLE_HUMIDITY == true || CAYENNE_LPP == true  
      HumTemp->GetHumidity(&humidity);
      HumTemp->GetTemperature(&temperatureFromHTS221);
      PressTemp->GetPressure(&pressure);
      PressTemp->GetTemperature(&temperatureFromLPS22HB);
      Acc->GetAxes(accelerometer);
      Acc->GetTemperature(&temperatureFromLSM303AGR);
      Mag->GetAxes(magnetometer);
      lpp.reset();
      
      // Channel 1 : Data from HTS221 (Humidity sensor)
      lpp.addRelativeHumidity(1, humidity);
      lpp.addTemperature(1, temperatureFromHTS221);
      
      // Channel 2 : Data from LPS22HB (Pressure sensor)
      lpp.addBarometricPressure(2, pressure);
      lpp.addTemperature(2, temperatureFromLPS22HB);
      
      // Channel 3 : Data from LSM303AGR (accelerometer/magnetometer/gyroscope)
      lpp.addAccelerometer(3, (float)accelerometer[0]/1000, (float)accelerometer[1]/1000, (float)accelerometer[2]/1000);
      lpp.addTemperature(2, temperatureFromLSM303AGR);
#endif  
  
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
  int status;
  
  if(CONFIRMED)   Serial1.print(" Uplink CONFIRMED on PORT ");
  else            Serial1.print(" Uplink UNCONFIRMED on PORT ");
  Serial1.println(PORT); 
  
#if ((ENABLE_HUMIDITY == false) && (CAYENNE_LPP == false)) 
    Serial1.print(" Sending Text : ");Serial1.print(frameTx);Serial1.print("\r\n");
    status = loraNode.sendFrame(frameTx, sizeof(frameTx), CONFIRMED, PORT);
  
#elif ((ENABLE_HUMIDITY == true) && (CAYENNE_LPP == false)) 
    humidityLora = (uint8_t)humidity;
    Serial1.print(" Sending humidity(%) : ");Serial1.print(humidityLora);Serial1.print("\r\n");
    status = loraNode.sendFrame((char*)&humidityLora, 1, CONFIRMED);

#else
    Serial1.println(" Sending all sensor data  with CAYENNE LPP (Low Power Payload) ");
    status = loraNode.sendFrame((char*)lpp.getBuffer(), lpp.getSize(), CONFIRMED);
#endif

  
  if(status == LORA_SEND_ERROR) {
    Serial1.println(" Send frame failed!!!");
  } else if(status == LORA_SEND_DELAYED) {
    Serial1.println(" Module is busy : \r\n * It's still trying to send data \r\n OR * \r\n * You are over your allowed duty cycle");
  } else {
    Serial1.println(" Waiting for Downlink...");
  }
}


void showConfig(void){
  Serial1.println("\r\n\r\n\r\n");
  Serial1.println("#######################################");
  Serial1.println("#### LoRaWAN Training Session #########");
  Serial1.println("#####Savoie Mont Blanc University #####\r\n");

  if(ACTIVATION_MODE == OTAA)     {Serial1.println(" * Activation Mode : OTAA");}
  else                            {Serial1.println(" * Activation Mode : ABP");}
  
  if(SEND_BY_PUSH_BUTTON == true) {Serial1.println(" * Send Frame with Push Button");}
  else                            {Serial1.print(" * Send Frame every ");Serial1.print((FRAME_DELAY<7000)?7000:FRAME_DELAY);Serial1.println("ms");}

  Serial1.print(" * Spreading Factor : SF");Serial1.print(SPREADING_FACTOR);Serial1.print("\r\n");

  if(ADAPTIVE_DR == true)         {Serial1.println(" * Adaptive Data Rate : ON");}
  else                            {Serial1.println(" * Adaptive Data Rate : OFF");}
  
  if(CONFIRMED == true)           {Serial1.println(" * Frame Confirmed");}
  else                            {Serial1.println(" * Frame Unconfirmed");}

  Serial1.print(" * Application PORT Number : ");Serial1.print(PORT);Serial1.print("\r\n");

  if      ( ENABLE_HUMIDITY == false && CAYENNE_LPP == false) { Serial1.print(" * Payload is Text : "); Serial1.println(frameTx);}
  else if ( ENABLE_HUMIDITY == true  && CAYENNE_LPP == false) { Serial1.println(" * Payload is 1 byte : Humidity sensor");}
  else {  Serial1.println(" * Payload : All Sensors with CAYENNE LPP Format");}

  if(LOW_POWER == true)           {Serial1.println(" * Low Power Mode : ON\r\n");}
  else                            {Serial1.println(" * Low Power Mode : OFF\r\n");}
}

void infoAfterActivation(void){
   if(SEND_BY_PUSH_BUTTON == 0){
    Serial1.print(" Frame will be sent every ");Serial1.print((FRAME_DELAY<7000)?7000:FRAME_DELAY);Serial1.println("ms\r\n");
  }
  else {
    Serial1.println(" Press Blue Button to send a Frame\r\n");
  }
  
}


void infoBeforeActivation(void){
  while(!loraNode.begin(&SerialLora, LORA_BAND_EU_868)) {
    Serial1.println("Lora module not ready");
    delay(1000);
  }
  loraNode.setDutyCycle(DISABLE);

 if (ACTIVATION_MODE == ABP) {
  Serial1.print(" * Device Address :          0x ");
  Serial1.println(devAddr);
  Serial1.print(" * Network Session Key :     0x ");
  Serial1.println(nwkSKey);
  Serial1.print(" * Application Session Key : 0x ");
  Serial1.println(appSKey);Serial1.print("\r\n");
 }
 
 if (ACTIVATION_MODE == OTAA){
  str = " * Device EUI :      0x ";
  loraNode.getDevEUI(&str);
  Serial1.println(str);
  Serial1.print(" * Application key : 0x ");
  Serial1.println(appKey);
  Serial1.print(" * Application EUI : 0x ");
  Serial1.println(appEUI);Serial1.print("\r\n");
 }
 
 loraNode.setAdaptativeDataRate(DISABLE);
 loraNode.setDataRate(12-SPREADING_FACTOR);
 if(ADAPTIVE_DR)   loraNode.setAdaptativeDataRate(ENABLE);       
 loraNode.setDutyCycle(DISABLE);
}


void activationABP(void){
  while(!loraNode.joinABP(devAddr, nwkSKey, appSKey)) {
    Serial1.println(" The Device is not activated on the Server!!");
    delay(1000);
  }
}

void activationOTAA(void){
  Serial1.println(" JOIN procedure in progress ...");  
  
  while(!loraNode.joinOTAA(appKey, appEUI)) {
    Serial1.println(" JOIN OTAA failed!!! Retry...");
    delay(1000);
  }
  Serial1.println(" JOIN procedure : SUCCESS !\r\n");
}
