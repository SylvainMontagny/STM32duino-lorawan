
#include "LoRaWANNode.h"
#include <HTS221Sensor.h>
#include <LPS22HBSensor.h>
#include <LSM303AGR_ACC_Sensor.h>
#include <LSM303AGR_MAG_Sensor.h>
#include <CayenneLPP.h>

#define I2C2_SCL    D15//PB10
#define I2C2_SDA    D14//PB11

#define SEND_BY_PUSH_BUTTON false     // Sending method (Time or Push Button)     
#define FRAME_DELAY         7000      // Time between 2 frames
#define DATA_RATE           5
#define ADAPTIVE_DR         false
#define CONFIRMED           false
#define PORT                1
#define CAYENNE_LPP         true

HardwareSerial SerialLora(D0, D1); // D0(Rx) D1(TX)
HardwareSerial Serial1(PA10, PA9);
TwoWire *dev_i2c;

HTS221Sensor  *HumTemp;
LPS22HBSensor *PressTemp;
LSM303AGR_ACC_Sensor *Acc;
LSM303AGR_MAG_Sensor *Mag;

// Initialise CAYENNE 
CayenneLPP lpp(51);

// The DevEUI is already in the Device and cannot be changed
const char appKey[] = "E24F43FFFE44CE1D7C96EF9AA9DF9ED8";
const char appEUI[] = "70B3D57ED0017552";


String str;
int8_t humidityLora =0;

// Read humidity, pressure, temperature from LPS22HB, temperature from HTS221
float pressure, humidity, temperatureFromHTS221, temperatureFromLPS22HB, temperatureFromLSM303AGR;
int32_t accelerometer[3];
int32_t magnetometer[3];

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PUSHBUTTON, INPUT);
  Serial1.begin(115200);

  // Initialize I2C bus.
  dev_i2c = new TwoWire(I2C2_SDA, I2C2_SCL);
  dev_i2c->begin();

  // Initialise CAYENNE 
  lpp.reset();

  // Initlialize components.
  HumTemp = new HTS221Sensor (dev_i2c);
  HumTemp->Enable();
  PressTemp = new LPS22HBSensor(dev_i2c);
  PressTemp->Enable();
  Acc = new LSM303AGR_ACC_Sensor(dev_i2c);
  Acc->Enable();
  Acc->EnableTemperatureSensor();
  Mag = new LSM303AGR_MAG_Sensor(dev_i2c);
  Mag->Enable();

  infoBeforeActivation();
  Serial1.println(" JOIN procedure in progress ...");  
  
  while(!loraNode.joinOTAA(appKey, appEUI)) {
    Serial1.println(" JOIN OTAA failed!!! Retry...");
    delay(1000);
  }
  
  Serial1.println(" JOIN procedure : SUCCESS !\r\n");
  if(SEND_BY_PUSH_BUTTON == 0){
    Serial1.print(" Frame will be sent every ");Serial1.print((FRAME_DELAY<7000)?7000:FRAME_DELAY);Serial1.println("ms\r\n");
  }
  else {
    Serial1.println(" Press Blue Button to send a Frame\r\n");
  }  
}

void loop()
{
 if( SEND_BY_PUSH_BUTTON == 1)   while(digitalRead(PUSHBUTTON)); // Attente Push Button pour envoyer
  else                            delay((FRAME_DELAY<7000)?0:FRAME_DELAY-7000);  // Attente FRAME_DELAY pour envoyer
  
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
  //lpp.addAccelerometer(3, float(accelerometer[0]), float(accelerometer[1]), float(accelerometer[2]));
  lpp.addAccelerometer(3, (float)accelerometer[0]/1000, (float)accelerometer[1]/1000, (float)accelerometer[2]/1000);
  lpp.addTemperature(2, temperatureFromLSM303AGR);

  //Serial1.print(" Humidity : ");Serial1.print(humidity, 1);Serial1.println("%");
  //Serial1.print(" Temperature : ");Serial1.print(temperatureFromHTS221, 1);Serial1.println("°C");
  
 
  
  transmit();
  receive();
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
    Serial1.println(" No data received\r\n");
  }
}

void transmit(void) {
 int status;
 if(CONFIRMED)   Serial1.print(" Uplink CONFIRMED on PORT ");
 else            Serial1.print(" Uplink UNCONFIRMED on PORT ");
 Serial1.println(PORT);
 
 if(CAYENNE_LPP == false){
  humidityLora = (uint8_t)humidity;
  Serial1.print(" Sending value : ");Serial1.println(humidityLora);
  status = loraNode.sendFrame((char*)&humidityLora, 1, CONFIRMED);
 }
 else{
   Serial1.println(" Sending CAYENNE LPP (Low Power Payload) ");
   status = loraNode.sendFrame((char*)lpp.getBuffer(), lpp.getSize(), CONFIRMED);
 }
 
  if(status == LORA_SEND_ERROR) {
    Serial1.println(" Send Frame failed!!!");
  } else if(status == LORA_SEND_DELAYED) {
    Serial1.println(" Module is busy : \r\n * It's still trying to send data \r\n OR * \r\n * You are over your allowed duty cycle");
  } else {
   Serial1.println(" Frame sent. Waiting for Downlink...");
  }
}

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
