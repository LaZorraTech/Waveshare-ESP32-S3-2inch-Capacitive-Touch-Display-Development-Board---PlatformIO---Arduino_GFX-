#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <CST816S.h>

#define LCD_SCLK 39
#define LCD_MOSI 38
#define LCD_MISO 40
#define LCD_DC 42
#define LCD_RST -1
#define LCD_CS 45

#define TFT_BL 1

//#define LCD_ROTATION 1
#define LCD_H_RES 240
#define LCD_V_RES 320
int lcdrotation = 3;

// Initialize SPI Bus
Arduino_DataBus *bus = new Arduino_ESP32SPI(  LCD_DC /* DC */, LCD_CS /* CS */, LCD_SCLK /* SCK */, LCD_MOSI /* MOSI */, LCD_MISO /* MISO */);

// Initialize Display
Arduino_GFX *gfx = new Arduino_ST7789(bus, LCD_RST, 0 ,true, LCD_H_RES, LCD_V_RES);

#define PIN_I2C_SDA 48
#define PIN_I2C_SCL 47
#define TOUCH_PIN_INTERRUPT 46
//#define TOUCH_PIN_RESET

CST816S touch(PIN_I2C_SDA, PIN_I2C_SCL, -1, TOUCH_PIN_INTERRUPT);	// sda, scl, rst, irq

uint32_t current_millis;
static uint32_t previous_millis = 0;
static uint32_t button_millis = 0;

int touchx = 0;
int touchy = 0;
int touchxtemp = 0;
int touchytemp = 0;
bool touchbool = 0;
String gesturestring;
const char * gesturechar = "gesturechar";
bool gesturebool = 0;
int pagenum = 1;

void page1display(){
  gfx->setTextSize(2,2,0);
  gfx->setTextColor(WHITE,DARKCYAN);

  gfx->setCursor(100, 20);
  gfx->print("Page 1");

  gfx->setCursor(100, 60);
  gfx->print("Swipe");

  gfx->setCursor(100, 100);
  gfx->print("---->");

}

void page2display(){
  gfx->setTextSize(2,2,0);
  gfx->setTextColor(WHITE,DARKCYAN);

  gfx->setCursor(100, 20);
  gfx->print("Page 2");

  gfx->setCursor(100, 60);
  gfx->print("Swipe");

  gfx->setCursor(100, 100);
  gfx->print("<----");

}

void gestureprocess(){
  if (pagenum == 1 && (strcmp(gesturechar, "RIGHT") == 0)){  //Change to Page 2
    gfx->fillScreen(DARKCYAN);
    previous_millis = current_millis;
    pagenum = 2;
  }

  if (pagenum == 2 && (strcmp(gesturechar, "LEFT") == 0)){  //Change to Page 1
    gfx->fillScreen(DARKCYAN);
    previous_millis = current_millis;
    pagenum = 1;
  }
}

void touchfunct(){
  if (touch.available()) {
    touchxtemp = touch.data.x;
    touchytemp = touch.data.y;
    gesturestring = (touch.gesture());
    gesturechar = gesturestring.c_str();
    touchbool = 1;
    //Serial.println("Touch.available");
  }
  
  if (touchbool == 1){   
    if (lcdrotation == 3){
      touchx = map(touchytemp, 320, 0, 0, 320);
      touchy = touchxtemp;
      //Serial.print("X: "); Serial.print(touchx);
      //Serial.print(" Y: "); Serial.println(touchy);


      //Serial.println(gesturechar);
      if(strcmp(gesturechar, "SWIPE DOWN") == 0){gesturechar = "RIGHT"; gesturebool = 1;}
      if(strcmp(gesturechar, "SWIPE UP") == 0){gesturechar = "LEFT"; gesturebool = 1;}
      if(strcmp(gesturechar, "SWIPE RIGHT") == 0){gesturechar = "DOWN";}
      if(strcmp(gesturechar, "SWIPE LEFT") == 0){gesturechar = "UP";}
    } 
    
    if((current_millis - button_millis) > 200){ //Button Debounce
      if(gesturebool == 1){
        gestureprocess();
      }
      button_millis = current_millis;
    }

    touchbool = 0;
  }
}

void setup() {
  Serial.begin(115200);

  gfx->begin();  // Init Display
  gfx->setRotation(lcdrotation);
  gfx->fillScreen(DARKCYAN);

  #ifdef TFT_BL
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
  #endif

  gfx->setTextSize(1);

  touch.begin(0x01);

}

void loop() {
  touchfunct();

  current_millis = millis();

  if (((current_millis - previous_millis) > 500) && (pagenum == 1)){
    page1display();
    previous_millis = current_millis;}

  if (((current_millis - previous_millis) > 500) && (pagenum == 2)){
    page2display();
    previous_millis = current_millis;}

}