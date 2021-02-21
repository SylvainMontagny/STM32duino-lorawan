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
}

// The loop function runs over and over again forever
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
    SerialLora.print(c);
  }
}
