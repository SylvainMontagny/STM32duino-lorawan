/*
  Allows to relay UART1 data from/to a PC terminal to/from LORA shield.
Connect RX USART1 (PA10-D2) au TX Virtual COM
Connect TX USART1 (PA9 -D8) au RX Virtual COM
*/
HardwareSerial SerialLora(D0, D1); // UART2(RX, TX)
HardwareSerial Serial1(PA10, PA9); // UART1(RX, TX)
void setup()
{
  Serial1.begin(115200);
  SerialLora.begin(115200);
  Serial1.println("\r\n\r\n\r\n");
  Serial1.println("########################################");
  Serial1.println("######## LoRaWAN Training Session ######");
  Serial1.println("#########     Bridge Serial     ########\r\n");
  Serial1.println(" Enter an AT command en press Enter (CR).\r\n");
}


void loop()
{
  char c;

  if (SerialLora.available() > 0)
  {
    c = SerialLora.read();
    Serial1.print(c);
  }
  if (Serial1.available() > 0)
  {
    c = Serial1.read();
    if( c=='\r') Serial1.println(); // For implicit CR/LF when a CR is received
    else    Serial1.print(c);       // For echoeing characters
    SerialLora.print(c);
  }
}
