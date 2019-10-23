#include <SPI.h>          // f.k. for Arduino-1.5.2
#include <SoftwareSerial.h>
//#include <Adafruit_GFX.h>// Hardware-specific library
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <TouchScreen.h>
#include <button.h>

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
SoftwareSerial trellis(51,49);

int h = tft.height();
int w = tft.width() * 2;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200);
  trellis.begin(115200);
  Serial.setTimeout(10);
  Serial1.setTimeout(10);
  Serial2.setTimeout(10);
  trellis.setTimeout(10);

  tft.reset();
  tft.begin(0x9486);
  tft.setRotation(1);
  //----------------------------
  String b = "";
  do
  {
    Serial1.println("ready?");
    delay(10);
    if(Serial1.available() > 0)
      b = Serial1.readString();
    Serial.println("waiting for motor 1");
  }while(b.indexOf("ready") == -1);
  //---------------------------
  b = "";
  do
  {
    Serial2.println("ready?");
    delay(10);
    if(Serial2.available() > 0)
      b = Serial2.readString();
    Serial.println("waiting for motor 1");
  }while(b.indexOf("ready") == -1);
  //----------------------------
  
  //homeMotors();
  mainMenu();
}

void tsCoord(int &x, int &y, int orientation)
{
  if(orientation == 1){
    int TS_MINX  = 73;
    int TS_MAXX  = 930;
    int  TS_MINY = 125;
    int  TS_MAXY =911;
    int a = x;
    x = y;
    y = a;
    x = x + y;       
    y = x - y;            
    x = x - y;   
    x = map(x, TS_MINX, TS_MAXX, 0, tft.width());
    y = (tft.height() - map(y, TS_MINY, TS_MAXY, tft.height(), 0));
  }
  else if(orientation == 0){
    int TS_MINX  = 121;
    int TS_MAXX  = 900;
    int  TS_MINY = 67;
    int  TS_MAXY  =940;
    x = x + y;       
    y = x - y;            
    x = x - y;   
    x = map(x, TS_MINX, TS_MAXX, tft.width(), 0);
    y = tft.height()-(map(y, TS_MINY, TS_MAXY, tft.height(), 0));
  }
}

void loop() {
  if(Serial1.available() > 0)
  {
    Serial.println("[Upper Arm]: " + Serial1.readString());
  }
  
  if(Serial2.available() > 0)
  {
    Serial.println("[Base]: " + Serial2.readString());
  }

  if(Serial.available() > 0)
  {
    Serial.println("I need to program this");
  }
}

void homeMotors()
{

  tft.fillScreen(BLACK);
  
  button homeButton;
  homeButton.set(w/2 - w/4, h/2 - h/4, w/2, h/2, WHITE, "Home Motors", 10, BLACK);
  homeButton.draw(tft);

  TSPoint p; 
  do
  {
    p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    tsCoord(p.x, p.y, 1);
  }while(!homeButton.isClick(p.x,p.y,1));

  homeButton.addBorder(RED, tft);
    
  Serial.println("homing...");
  tft.fillScreen(BLACK);
  button b;
  b.set(w/2 - w/4, h/2 - h/6, w/2, h/3, WHITE, "Homing Motor 1", 0, RED);
  b.draw(tft);
  Serial1.println("home");
  String d = "";
  do
  {
    if(Serial1.available() > 0)
      d = Serial1.readString();
  }while(d.indexOf("done") == -1);

  b.set(w/2 - w/4, h/2 - h/6, w/2, h/3, WHITE, "Homing Motor 2", 0, RED);
  b.draw(tft);
  Serial2.println("home");
  d = "";
  
  do
  {
    if(Serial2.available() > 0)
      d = Serial2.readString();
  }while(d.indexOf("done") == -1);
  
  Serial.println("Done");
  
  b.text = "Motors are homed";
  b.textColor = GREEN;
  b.draw(tft);
  delay(1000);
    
  mainMenu();
  
}

void mainMenu()
{
  tft.fillScreen(BLACK);
  
  button program, manual, homeButton;
  program.set(w/20, h/6, w/2 - w/10, 2 * h / 3, BLUE, "Programs", 10, BLACK);
  program.draw(tft);
  manual.set(w/2 + w/20, h/6, w/2 - w/10,  2 * h / 3, MAGENTA, " Manual ", 10, BLACK);
  manual.draw(tft);
  homeButton.set(w/3, h - h/10, w/3, h/10, WHITE, "Home Axis", 10, BLACK);
  homeButton.draw(tft);

  TSPoint p; 
  do
  {
    p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    tsCoord(p.x, p.y, 1);

    if(Serial.available() > 0)
    {
      String g = Serial.readString();
      Serial.println(g);
      if(g.indexOf("m1") != -1)
      {
        g = g.substring(g.indexOf("m1") + 2);
        Serial.println(g);
        Serial1.println(g);
        g = "";
        do
        {
          if(Serial1.available()>0)
          {
            g = Serial1.readString();
            Serial.println(g);
          }
        }while(g.indexOf("done") == -1);
      }
      else if(g.indexOf("m2") != -1)
        Serial2.println(g);
    }
  }while(!program.isClick(p.x,p.y,1) && !manual.isClick(p.x,p.y,1) && !homeButton.isClick(p.x,p.y,1));
  if(program.isClick(p.x,p.y,1))
  {
    program.addBorder(CYAN, tft);
    delay(100);
    runProgram();
  }
  else if(manual.isClick(p.x,p.y,1))
  {
    manual.addBorder(RED, tft);
    delay(100);
    manualMode();
  }
  else
  {
    homeButton.addBorder(YELLOW, tft);
    delay(100);
    homeMotors();
  }
}

void runProgram()
{
  tft.fillScreen(BLUE);
}

void manualMode()
{
  tft.fillScreen(BLACK);

  
  String b = "";
  do
  {
    trellis.println("ready?");
    delay(10);
    if(trellis.available() > 0)
      b = trellis.readString();
  }while(b.indexOf("ready") == -1);

  trellis.println("start");
  do
  {
    delay(10);
    if(trellis.available() > 0)
      b = trellis.readString();
  }while(b.indexOf("done") == -1);
  
  button back, m1, m2;
  //tft.fillRect(0,0, w, h/10, BLACK);
  back.set(0, 0, w/5, h/10, BLUE, "Return", 10, BLACK);
  back.draw(tft);
  m1.set(w/8, h/10 + (2 * (h - h/10) / 8)/3, 3*w/4, 3 * (h - h/10) / 8, RED, "Motor 1", 10, BLACK);
  m1.draw(tft);
  m1.addBorder(BLUE, tft);
  m2.set(w/8,h/10 + (2 * (h - h/10) / 8)/3 + (h - h/10) / 2, 3*w/4, 3 * (h - h/10) / 8, RED, "Motor 2", 10, BLACK);
  m2.draw(tft);

  Serial.println((2 * (h - h/10) / 8)/3);
  int selected = 1;
  const int in = 100;

  TSPoint p; 
  do
  {
    p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    tsCoord(p.x, p.y, 1);

    if(m1.isClick(p.x,p.y,1))
    {
      m1.addBorder(BLUE, tft);
      m2.removeBorder(tft);
      selected = 1;
    }
    else if(m2.isClick(p.x,p.y,1))
    {
      m2.addBorder(BLUE, tft);
      m1.removeBorder(tft);
      selected = 2;
    }

    if(trellis.available() > 0)
    {
      String t = trellis.readString();

      if(t.indexOf("-1") != -1)
      {
        if(selected == 1)
        {
          Serial2.println("mr:-" + String(in));
          do
          {
            if(Serial2.available() > 0)
              t = Serial2.readString();
          }while(t.indexOf("done") == -1);
        }
        else
        {
          Serial1.println("mr:-" + String(in));
          do
          {
            if(Serial1.available() > 0)
              t = Serial1.readString();
          }while(t.indexOf("done") == -1);
        }
      }
      else if(t.indexOf("1") != -1)
      {
        if(selected == 1)
        {
          Serial2.println("mr:" + String(in));
          do
          {
            if(Serial2.available() > 0)
              t = Serial2.readString();
          }while(t.indexOf("done") == -1);
        }
        else
        {
          Serial1.println("mr:" + String(in));
          do
          {
            if(Serial1.available() > 0)
              t = Serial1.readString();
          }while(t.indexOf("done") == -1);
        }
      }
      else if(t.indexOf("-2") != -1)
      {
        if(selected == 1)
        {
          Serial2.println("move:0");
          do
          {
            if(Serial2.available() > 0)
              t = Serial2.readString();
          }while(t.indexOf("done") == -1);
        }
        else
        {
          Serial1.println("move:0");
          do
          {
            if(Serial1.available() > 0)
              t = Serial1.readString();
          }while(t.indexOf("done") == -1);
        }
      }
      else if(t.indexOf("2") != -1)
      {
        if(selected == 1)
        {
          Serial2.println("move:4500");
          do
          {
            if(Serial2.available() > 0)
              t = Serial2.readString();
          }while(t.indexOf("done") == -1);
        }
        else
        {
          Serial1.println("move:24000");
          do
          {
            if(Serial1.available() > 0)
              t = Serial1.readString();
          }while(t.indexOf("done") == -1);
        }
      }
      
    }
  }while(!back.isClick(p.x,p.y,1));

  b = "";
  trellis.println("stop");
  do
  {
    delay(10);
    if(trellis.available() > 0)
      b = trellis.readString();
  }while(b.indexOf("done") == -1);

  mainMenu();
}

