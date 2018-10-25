/*Written By: Er. Dapinder Singh Virk
  Email ID: brillosolutions@gmail.com
  Version: 1.0.0
  Date: October 25, 2018
  Outputs: Transmits toggled 1-Byte data at every iteration after time delay of 1 second. */

#include <SPI.h>
#define CE_pin 9
#define CSN_pin 10
#define MOSI_pin 11
#define MISO_pin 12
#define SCK_pin 13

#define CONFIG 0x00
#define STATUS 0x07
#define FLUSH_TX B11100001
#define W_TX_PAYLOAD B10100000

uint8_t dataByte = 0x55;
void setup() 
{
  Serial.begin(9600);
  SPI.begin();
  pinMode(CE_pin, OUTPUT);                                  //  Setting CE pin as output.
  pinMode(CSN_pin, OUTPUT);                                 //  Setting CSN pin as output.
  pinMode(MOSI_pin, OUTPUT);                                //  Setting MOSI pin as output.
  pinMode(MISO_pin, INPUT);                                 //  Setting MISO pin as input.
  pinMode(SCK_pin, OUTPUT);                                 //  Setting SCK pin as output.
  digitalWrite(CSN_pin, HIGH);                              //  Setting CSN = 1 to make SPI idle.
  writeReg(CONFIG, 0x0A);                                   //  Setting PRIM_RX = 0 and PWR_UP = 1 in CONFIG register to put radio in TX and standby mode (I) or (II).
}

void loop() 
{
  digitalWrite(CE_pin, HIGH);                               // Making radio ready to get into TX/RX mode. 
  sendPayload(dataByte);                                    // Sending payload over the radio link to be assembled in packet.
  Serial.println(dataByte, HEX);                            // Showing same payload on serial terminal.
  dataByte = ~dataByte;                                     // Toggling the byte at every iteration.
  digitalWrite(CE_pin, LOW);                                // Falling back to standby mode (I) or (II).
  delay(1000);                                              // Delay of 1 second after each iteration.
}

void writeReg(byte ADDR, byte CONTENT)
{
  digitalWrite(CSN_pin, LOW);                               // Setting CSN = 0 to make SPI active.                              
  SPI.transfer(32 + ADDR);                                  // Address of register where content is to be written.
  SPI.transfer(CONTENT);                                    // Actual content that is to be written.
  digitalWrite(CSN_pin, HIGH);                              // Setting CSN = 0 to make SPI idle.
}

void flushTx()
{
  digitalWrite(CSN_pin, LOW);                               // Setting CSN = 0 to make SPI active. 
  SPI.transfer(FLUSH_TX);                                   // Making TX FIFO empty by using command: FLUSH_TX.
  digitalWrite(CSN_pin, HIGH);                              // Setting CSN = 0 to make SPI idle.
}

void sendPayload(byte payloadByte)
{
  flushTx();                                                // Flushing the TX FIFO.
  digitalWrite(CSN_pin, LOW);                               // Setting CSN = 0 to make SPI active.
  SPI.transfer(W_TX_PAYLOAD);                               // Writing the payload in TX FIFO by using command: W_TX_PAYLOAD.
  SPI.transfer(payloadByte);                                // Payload byte that is to be sent by radio over the air.
  digitalWrite(CSN_pin, HIGH);                              // Setting CSN = 0 to make SPI idle.
  writeReg(STATUS, 0x70);                                   // Clearing all interrupts including TX_DS in STATUS register.
}
