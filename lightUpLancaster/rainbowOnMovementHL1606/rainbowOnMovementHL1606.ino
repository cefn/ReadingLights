#include "HL1606stripPWM.h"

int triggerPin = 12;
int echoPin = 9;
long maxPulse = 16000; //longest time a pulse might take in microseconds
long displayTime = 10000; //time to display after an ultrasonic trigger in milliseconds

unsigned long lastPresence = 0; //timecode at which ultrasonic last saw movement
unsigned long lastTriggered = 0; //timecode at which sequence was initiated (there's hysteresis which prevents immediate re-triggering)

byte maxByte = 32;

byte BLACK[] = {0,0,0};
byte WHITE[] = {maxByte,maxByte,25};
byte RED[] = {maxByte,0,0};
byte PINK[] = {maxByte,maxByte/4,maxByte/4};
byte ORANGE[] = {maxByte,maxByte/6,0};
byte GREEN[] = {0,maxByte,0};
byte LIGHT_BLUE[] = {13,25,32};
byte BLUE[] = {0,0,maxByte};
byte YELLOW[] = {maxByte,12,0};

//DAWN: Blue, Pastel Blue, Orange
//DAY: Orange, Yellow, White
//DUSK: White, Yellow, Orange
//NIGHT: Orange, Pastel Blue, Blue

/*
//DAWN
byte *START = BLUE;
byte *MIDDLE = LIGHT_BLUE;
byte *END = PINK;

//DAY
byte *START = PINK;
byte *MIDDLE = YELLOW;
byte *END = WHITE;


//DUSK
byte *START = LIGHT_BLUE;
byte *MIDDLE = YELLOW;
byte *END = ORANGE;
*/

//NIGHT
byte *START = ORANGE;
byte *MIDDLE = LIGHT_BLUE;
byte *END = BLUE;

/*

*/

int latchPin = 10;
HL1606stripPWM strip = HL1606stripPWM(32, latchPin); 

void setup(){
  
  Serial.begin(115200);
  
  //configure pins for Ultrasonic sensor
  pinMode(triggerPin,OUTPUT);
  pinMode(echoPin,INPUT);
  
  strip.setPWMbits(5);
  strip.setSPIdivider(16);
  strip.setCPUmax(70);    // 70% is what we start at
  
  strip.begin();
  turnOff();
  
}

void loop(){
  //Serial.print("<");

  long duration, distance;

  //get a distance sensor value
  
  //trigger pulse 
  digitalWrite(triggerPin,LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin,LOW);
  
  //wait on response
  duration = pulseIn(echoPin, HIGH, maxPulse);
  distance = (duration/2) / 29.1;
  //Serial.print("Distance");
  Serial.print(distance);
  
  //TODO CH REMOVE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //distance = 100;
  ///THE ABOVE LINE OVERRIDES THE SENSOR

  
  //time since last triggered
  unsigned long since = (millis() - lastTriggered);
  //Serial.print(" since: ");
  //Serial.println(since);
    
  if(distance > 0 && distance < 140 ){
    lastPresence = millis();
    //Serial.print("D");
    if((lastTriggered == 0) || (since > (displayTime * 2))){
      lastTriggered = millis();
      //Serial.print("T");
    } 
  }
  
  boolean glowing=false;
  if(lastTriggered != 0){ //has been triggered at least once
    if(since < displayTime){ //triggered recently
      glowing = true;
    }
  }
  
  if(glowing){
    unsigned long halfTime = displayTime / 2;
    unsigned long halfSince = since % halfTime;
    int pos = ((halfSince % halfTime) * 256) / halfTime;
    float brightness = 1.0f;
    if(since < halfTime){
      brightness = min(1.0f,((float)pos) / 256.0f * 16.0f);
      paintTween(START,MIDDLE,pos, brightness);
    }
    else{
      brightness = min(1.0f,(255.0f - ((float)pos)) / 256.0f * 16.0f);
      paintTween(MIDDLE,END,pos, brightness);      
    }
    //paintColor5Bit(BLACK);
    //paintColor5Bit(BLUE);
    //paintColor5Bit(LIGHT_BLUE);
    //paintColor5Bit(ORANGE);
    //paintColor5Bit(YELLOW);

    //Serial.print("P");
  }
  else{
    //Serial.println("Off");
    turnOff();
  }
  
  //Serial.println(">");
  
}

/*
* Alternatives White Light, Red Light, Green Light, Blue Light
*/

void turnOff(){
  paintColor(0x000000);
}

void paintColor5Bit(byte *color){
  paintTween(color,color,0, 1.0f);
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

/** Tweens colors (as an r,g,b triplet between 0 and 255), with a position value (between 0 and 255)
* and a brightness between 0.0 and 1.0
*/
void paintTween(byte from[], byte to[], float pos, float brightness){
  
  float negpos = (255.0f - pos);
  float r = ((from[0] * negpos) + (to[0] * pos)) / 256.0f * brightness;
  float g = ((from[1] * negpos) + (to[1] * pos)) / 256.0f * brightness;
  float b = ((from[2] * negpos) + (to[2] * pos)) / 256.0f * brightness;
  paintColor(Color((byte)r,(byte)g,(byte)b));

  static int last = 0;
  if(brightness != last){
    last = brightness;
    Serial.print("pos:");
    Serial.print(pos);
    Serial.print(",");
    Serial.print(" bright:");
    Serial.print(brightness);
    Serial.print(" from:");
    Serial.print(from[0]);
    Serial.print(",");
    Serial.print(from[1]);
    Serial.print(",");
    Serial.print(from[2]);
    Serial.print(" to:");
    Serial.print(to[0]);
    Serial.print(",");
    Serial.print(to[1]);
    Serial.print(",");
    Serial.print(to[2]);
    Serial.print(" rgb:");
    Serial.print((byte)r);
    Serial.print(",");
    Serial.print((byte)g);
    Serial.print(",");
    Serial.println((byte)b);    
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
