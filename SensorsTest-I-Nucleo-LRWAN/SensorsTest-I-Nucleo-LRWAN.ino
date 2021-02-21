
#include <HTS221Sensor.h>
#include <LPS22HBSensor.h>
#include <LSM303AGR_ACC_Sensor.h>
#include <LSM303AGR_MAG_Sensor.h>

#define I2C2_SCL    D15//PB10
#define I2C2_SDA    D14//PB11
HardwareSerial Serial1(PA10, PA9);
TwoWire *dev_i2c;

// Components.
HTS221Sensor  *HumTemp;
LPS22HBSensor *PressTemp;
LSM303AGR_ACC_Sensor *Acc;
LSM303AGR_MAG_Sensor *Mag;


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial1.begin(115200);

  // Initialize I2C bus.
  dev_i2c = new TwoWire(I2C2_SDA, I2C2_SCL);
  dev_i2c->begin();

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

  Serial1.println("\r\n\r\n\r\n");
  Serial1.println("########################################");
  Serial1.println("######## LoRaWAN Training Session ######");
  Serial1.println("#########     WELCOME !!!!!!!   ########\r\n");
  Serial1.println("\r\n  It seems ideal conditions for work !!! Let's start !\r\n");

}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(250);
  digitalWrite(LED_BUILTIN, LOW);
  delay(2500);

  // Read humidity, pressure, temperature from LPS22HB, temperature from HTS221
  float pressure, humidity, temperatureFromHTS221, temperatureFromLPS22HB, temperatureFromLSM303AGR;
  int32_t accelerometer[3];
  int32_t magnetometer[3];
  HumTemp->GetHumidity(&humidity);
  HumTemp->GetTemperature(&temperatureFromHTS221);
  PressTemp->GetPressure(&pressure);
  PressTemp->GetTemperature(&temperatureFromLPS22HB);
  Acc->GetAxes(accelerometer);
  Acc->GetTemperature(&temperatureFromLSM303AGR);
  Mag->GetAxes(magnetometer);
  
  Serial1.print(" Humidity : ");
  Serial1.print(humidity, 2);
  Serial1.print("%   |   Temperature[HTS221] : ");
  Serial1.print(temperatureFromHTS221, 2);
  Serial1.print("C   |   Presure : ");
  Serial1.print(pressure, 2);
  Serial1.print("hPa   |   Temperature[LPS22HB] : ");
  Serial1.print(temperatureFromLPS22HB, 2);
  Serial1.println("C");

  Serial1.print(" Accelerometer [mg]     : ");
  Serial1.print(accelerometer[0]);
  Serial1.print("(x) ");
  Serial1.print(accelerometer[1]);
  Serial1.print("(y) ");
  Serial1.print(accelerometer[2]);
  Serial1.println("(z)");
  Serial1.print(" Magnetometer [mGauss]  : ");
  Serial1.print(magnetometer[0]);
  Serial1.print("(x) ");
  Serial1.print(magnetometer[1]);
  Serial1.print("(y) ");
  Serial1.print(magnetometer[2]);
  Serial1.println("(z)");
  Serial1.print(" Temperature[LSM303AGR] : ");
  Serial1.print(temperatureFromLSM303AGR, 2);
  Serial1.println("C");
  Serial1.println("");
}
