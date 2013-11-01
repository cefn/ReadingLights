#include "HL1606stripPWM.h"
#include <util/delay.h>

int triggerPin = 12;
int echoPin = 9;
long maxDuration = 160000; //longest time a pulse might take in microseconds

int latchPin = 10;
HL1606stripPWM strip = HL1606stripPWM(2, latchPin); 

void setup(){
  
  Serial.begin(115200);
  
  //configure pins for Ultrasonic sensor
  pinMode(triggerPin,OUTPUT);
  pinMode(echoPin,INPUT);
  digitalWrite(echoPin, HIGH); //pull up resistor
  
  strip.setPWMbits(3);
  strip.setSPIdivider(16);
  strip.setCPUmax(1);
  
  strip.begin();
  
}

void loop(){

  //alternative delay function  
    /*
  unsigned int us;
  us = 8;  this is equivalent to 2 ms
  __asm__ __volatile__ (
          "1: sbiw %0,1" "\n\t" // 2 cycles
          "brne 1b" : "=w" (us) : "0" (us) // 2 cycles
  );
  */
  
  int counter;
  digitalWrite(triggerPin,LOW);
  _delay_us(2);
  digitalWrite(triggerPin,HIGH);
  _delay_us(10);
  digitalWrite(triggerPin,LOW);
  
  //wait on response
  
  unsigned long distance = 0;
  unsigned long duration = 0;
  while(digitalRead(echoPin) == LOW){
    //do nothing
  };
  unsigned long pulseStart = micros();  
  while(digitalRead(echoPin) == HIGH){
  }  
  duration = micros() - pulseStart;
  
  //duration = pulseIn(echoPin, HIGH, maxDuration);
  static unsigned long last = 0;
  distance = (duration/2) / 29.1;
  if(distance != last){
    Serial.println(distance);
    last = distance;
  }
}
