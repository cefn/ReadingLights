#include "HL1606stripPWM.h"

int triggerPin = 12;
int echoPin = 9;
long maxDuration = 50000; //longest time a pulse might take in microseconds
long displayTime = 1000; //time to display after an ultrasonic trigger in milliseconds

unsigned long lastPresence = 0; //timecode at which ultrasonic last saw movement
unsigned long lastTriggered = 0; //timecode at which sequence was initiated (there's hysteresis which prevents immediate re-triggering)

uint32_t WHITE = Color(255,255,255);
uint32_t RED = Color(255,0,0);
uint32_t GREEN = Color(0,255,0);
uint32_t BLUE = Color(0,0,255);

uint32_t staticColor = WHITE;

int latchPin = 10;
HL1606stripPWM strip = HL1606stripPWM(2, latchPin); 

void setup(){
  
  Serial.begin(115200);
  
  //configure pins for Ultrasonic sensor
  pinMode(triggerPin,OUTPUT);
  pinMode(echoPin,INPUT);
  
  strip.setPWMbits(3);
  strip.setSPIdivider(16);
  strip.setCPUmax(10);    // 70% is what we start at
  
  //strip.begin();
  //turnOff();
  
}

void loop(){
  Serial.print("<");
  
  //get a distance sensor value
  
  //trigger pulse 
  digitalWrite(triggerPin,LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin,LOW);
  
  //wait on response
  long duration, distance;
  duration = pulseIn(echoPin, HIGH, maxDuration);
  distance = (duration/2) / 29.1;
  //Serial.print("Distance");
  Serial.print(distance);
  
  //time since last triggered
  unsigned long since = (millis() - lastTriggered);
  
  if(distance > 0 && distance < 140 ){
    lastPresence = millis();
    Serial.print("D");
    if(lastTriggered == 0 || since > displayTime * 2){
      lastTriggered = millis();
      Serial.print("T");
    } 
  }
      
  if(lastTriggered != 0){ //has been triggered at least once
    if(since < displayTime){ //triggered recently
      paintColor(staticColor);
      Serial.print("P");
    }
    else{
      Serial.print("O");
      turnOff();
    }
  }
 
  Serial.println(">");
}

/*
* Alternatives White Light, Red Light, Green Light, Blue Light
*/

void turnOff(){
  paintColor(0x000000);
}

void paintColor(uint32_t c){
  for (int i=0; i < strip.numLEDs(); i++) {
    strip.setLEDcolorPWM(i, (c & 0x1F) << 3, ((c>>10) & 0x1F) << 3, ((c>>5) & 0x1F) << 3);
  }
}

void paintRainbow(int pos){
  for (int i=0; i < strip.numLEDs(); i++) {
    uint16_t c = Wheel((i+pos) % 96);
    strip.setLEDcolorPWM(i, (c & 0x1F) << 3, ((c>>10) & 0x1F) << 3, ((c>>5) & 0x1F) << 3);
  }
}

unsigned int Color(byte r, byte g, byte b)
{
  //Take the lowest 5 bits of each value and append them end to end
  return( ((unsigned int)g & 0x1F )<<10 | ((unsigned int)b & 0x1F)<<5 | (unsigned int)r & 0x1F);
}

//Input a value 0 to 127 to get a color value.
//The colours are a transition r - g -b - back to r
unsigned int Wheel(byte WheelPos)
{
  byte r,g,b;
  switch(WheelPos >> 5)
  {
    case 0:
      r=31- WheelPos % 32;   //Red down
      g=WheelPos % 32;      // Green up
      b=0;                  //blue off
      break; 
    case 1:
      g=31- WheelPos % 32;  //green down
      b=WheelPos % 32;      //blue up
      r=0;                  //red off
      break; 
    case 2:
      b=31- WheelPos % 32;  //blue down 
      r=WheelPos % 32;      //red up
      g=0;                  //green off
      break; 
  }
  return(Color(r,g,b));
}
