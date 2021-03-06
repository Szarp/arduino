/*
 Copyright (C) 2015 Dariusz Mazur, <darekm@emadar.com>

 */

/**
 * Simplest possible example of using RF24Network 
 *
 * SENSOR NODE - Soil moisure
 * Every 2 seconds, send a payload to the receiver node.
 */

//#include <RF24Network.h>
#include "me_base.h"
#include <RF24.h>
#include <SPI.h>
#include "printf.h"

#include "LowPower.h"
//https://github.com/oransel/arduino-low-power

#include <avr/power.h>
#include <me_frame.h>
#include "me_soil.h"

RF24 radio(9,10);                    // nRF24L01(+) radio attached using Getting Started board 
Frame frame;

//RF24Network network(radio);          // Network uses that radio

const uint16_t this_node = 01;        // Address of our node in Octal format
const uint16_t other_node = 00;       // Address of the other node in Octal format
int moistPin = A0;
int moistOut = 3;


const unsigned long interval = 2; //ms  // How often to send 'hello world to the other unit

unsigned long last_sent;             // When did we last send?
unsigned long packets_sent;          // How many have we sent already
int ledPin = 13;      // select the pin for the LED
int sensorValue = 0;  // variable to store the value coming from the sensor



#ifdef _ME_Mega_
#define _ME_Serial_
#endif




// http://code.google.com/p/tinkerit/wiki/SecretVoltmeter
long internalVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}

long internalTemp() {
//  https://code.google.com/p/tinkerit/wiki/SecretThermometer
  long result;
  // Read temperature sensor against 1.1V reference
  ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(MUX3);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = (result - 125) * 1075;
  return result;
}

int moistValue(){
   return analogRead(moistPin);    

}

void printOk(bool ok){
    if (ok)
      Serial.println("ok.");
    else {
      String s("failed.");
      s.setCharAt(1,'A');
      s.setCharAt(2,0);
      Serial.println(s);
    }  
//     delay(100);

}  

void printPayload(MemsValue_t & p ){
    Serial.print(" action:");
    Serial.print(p.action);
    Serial.print(" time:");
    Serial.print(p.time);
    Serial.print(frame.ToString(p));
   // delay(200);
  
}

void sendFrame(MemsValue_t p){
  Serial.println("++");
  Serial.print(frame.ToFrame(p)); 
  Serial.println(":::");
  frame.write(p);
  Serial.println("--");
  Serial.write((const unsigned char *)&p,sizeof(MemsValue_t));
  Serial.println("==");
}
void setup(void)
{
  #ifdef _ME_Serial_
  Serial.begin(57600);
  Serial.println("Metium/ moisture sensor");
 // Serial.println(_AVR_CPU_NAME_);
  #endif
  SPI.begin();
  radio.begin();
  Serial.println("St");
  frame.setStream(Serial);
//  network.begin(/*channel*/ 90, /*node address*/ this_node);
 radio.setAutoAck(true);
 #ifdef   _ME_Mega_
 // radio.printDetails();
 #endif
  pinMode(ledPin, OUTPUT);
  pinMode(moistOut,INPUT); 
  power_twi_disable();
  power_timer1_disable();
 Serial.flush(); 

}

void loop() {
  
//  network.update();                          // Check the network regularly
//LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF); 
   LowPower.idle(SLEEP_2S, ADC_OFF, TIMER2_OFF, TIMER1_ON, TIMER0_OFF, 
                SPI_OFF, USART0_OFF, TWI_ON);
  
  unsigned long now = millis();              // If it's time to send a message, send it!
    Serial.print("A");
    Serial.print(now);
    Serial.println("A");
//    delay(100);
  if ( now - last_sent >= interval  )
  {
    last_sent = now;
  #ifdef _ME_Serial_
    Serial.print("Send...");
  #endif  

    MemsValue_t payload = { last_sent,154, packets_sent++ };
    payload.value[0]=moistValue();
    payload.value[1]=internalVcc();
    payload.value[2]=internalTemp();
    bool ok;
    ok=false;
  #ifdef _ME_Serial_
    printPayload(payload);
    sendFrame(payload);
    printOk(ok);
 
  #endif  
//    RF24NetworkHeader header(/*to node*/ other_node);
//    bool ok = network.write(header,&payload,sizeof(payload));

  }
  Serial.flush();
//   LowPower.idle(SLEEP_1S, ADC_ON, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, 
//                SPI_ON, USART0_ON, TWI_OFF);

}


