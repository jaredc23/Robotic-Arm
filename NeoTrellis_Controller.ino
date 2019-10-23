#include "Adafruit_NeoTrellis.h"

#define Y_DIM 4 //number of rows of key
#define X_DIM 8 //number of columns of keys

bool enabled = false;

int layout[] = {0,0,1,0,0,-1,0,0,
                2,1,1,0,0,-1,-1,-2,
                2,1,1,0,0,-1,-1,-2,
                0,0,1,0,0,-1,0,0};

Adafruit_NeoTrellis t_array[Y_DIM/4][X_DIM/4] = {
  
  { Adafruit_NeoTrellis(0x2E), Adafruit_NeoTrellis(0x2F) }
  
};

Adafruit_MultiTrellis trellis((Adafruit_NeoTrellis *)t_array, Y_DIM/4, X_DIM/4);

uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return seesaw_NeoPixel::Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return seesaw_NeoPixel::Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return seesaw_NeoPixel::Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  return 0;
}

TrellisCallback blink(keyEvent evt){
  int key = evt.bit.NUM;

  if(abs(layout[key]) != 1 && abs(layout[key]) != 2)
    return 0;
  
  if(evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING)
  {
     for(int j = 0; j < 4; j++)
      for(int i = 0; i < 8; i++)
        if(layout[j * 8 + i] == layout[key])
          trellis.setPixelColor(i,j,0x220000);

     Serial.println(layout[key] * -1);
  }
  else if(evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING)
  {
      for(int j = 0; j < 4; j++)
      for(int i = 0; i < 8; i++)
        if(layout[j * 8 + i] == layout[key])
          if(abs(layout[j * 8 + i]) == 1)
            trellis.setPixelColor(i,j, 0x0000FF);
          else if(abs(layout[j * 8 + i]) == 2)
            trellis.setPixelColor(i, j, 0x00FF00);
  }
  
  trellis.show();
  return 0;
}

int findValInArray(int arr[], int key)
{
  for(int i = 0; i < sizeof(arr); i++)
  {
    if(arr[i] == key)
      return i;
  }

  return 0;
}

void serialEvent() { //This function handles when there is messages sent through the serial port.
  String b = Serial.readString();
  if(b.indexOf("ready?") != -1)//This is used to check if the Arduino is responsive, without moving anything
  {
    Serial.println("ready");
  }
  else if(b.indexOf("start") != -1)
  {
    Serial.println("starting");
    initTrellis();
    Serial.println("done");
  }
  else if(b.indexOf("stop") != -1)
  {
    Serial.println("stopping");
    stopTrellis();
    Serial.println("done");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(10);

  if(!trellis.begin()){
    Serial.println("Keypad Not Responding, please reset board");
    while(1);
  }
  
  for(int y=0; y<Y_DIM; y++){
    for(int x=0; x<X_DIM; x++){
      //activate rising and falling edges on all keys
      trellis.activateKey(x, y, SEESAW_KEYPAD_EDGE_RISING, true);
      trellis.activateKey(x, y, SEESAW_KEYPAD_EDGE_FALLING, true);
      trellis.registerCallback(x, y, blink);
      trellis.setPixelColor(x, y, 0x000000); //addressed with x,y
      trellis.show(); //show all LEDs
    }
  }
}

void initTrellis()
{

  for(int i=0; i<Y_DIM*X_DIM; i++){
      trellis.setPixelColor(i, Wheel(map(i, 0, X_DIM*Y_DIM, 0, 255))); //addressed with keynum
      trellis.show();
      delay(50);
  }
  
  for(int y=0; y<Y_DIM; y++){
    for(int x=0; x<X_DIM; x++){
      //activate rising and falling edges on all keys
      trellis.activateKey(x, y, SEESAW_KEYPAD_EDGE_RISING, true);
      trellis.activateKey(x, y, SEESAW_KEYPAD_EDGE_FALLING, true);
      trellis.registerCallback(x, y, blink);
      trellis.setPixelColor(x, y, 0x000000); //addressed with x,y
      trellis.show(); //show all LEDs
      delay(10);
    }
  }
  
  for(int j = 0; j < 4; j++)
  {
    for(int i = 0; i < 8; i++)
    {
      if(abs(layout[j * 8 + i]) == 1)
        trellis.setPixelColor(i,j, 0x0000FF);
      else if(abs(layout[j * 8 + i]) == 2)
        trellis.setPixelColor(i, j, 0x00FF00);
    }
    trellis.show();
    delay(100);
  }

  enabled = true;
}

void stopTrellis()
{
  for(int y=Y_DIM - 1; y>=0; y--){
    for(int x=X_DIM - 1; x>=0; x--){
      trellis.setPixelColor(x, y, 0x000000); //addressed with x,y
    }
    trellis.show();
    delay(100);
  }

  enabled = false;
}

void loop() {
  if(Serial.available() > 0)
  {
    serialEvent();
  }

  if(enabled)
  {
    trellis.read();
    delay(20);
  }
}
