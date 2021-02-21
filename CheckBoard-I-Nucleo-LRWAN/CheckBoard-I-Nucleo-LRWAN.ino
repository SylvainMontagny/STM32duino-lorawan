#include "LoRaWANNode.h"

HardwareSerial SerialLora(D0, D1); // D0(Rx) D1(TX)
HardwareSerial Serial1(PA10, PA9);

void setup()
{
  Serial1.begin(115200);
  Serial1.println("\r\n\r\n\r\n");
  Serial1.println("########################################");
  Serial1.println("######## LoRaWAN Training Session ######");
  Serial1.println("########     Check Board          ######\r\n");

  Serial1.println(" Connecting to the LoRa Module ...\r\n");
  // Enable the USI module and set the radio band.
  while (!loraNode.begin(&SerialLora, LORA_BAND_EU_868)) {
    Serial1.println("Lora module not ready");
    delay(1000);
  }
  Serial1.println(" Connected\r\n");
  String str;
  str = " Unique DevEUI: 0x";
  loraNode.getDevEUI(&str);
  Serial1.println(str);

}

void loop()
{
  //empty loop
}
