#define TFT_DC PB1
#define TFT_CS PB0

stRadio radio;
stRadio old;

EasyTransfer ET; 
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

#define LED PC13
#define LED_ON digitalWrite(LED,LOW)
#define LED_OFF digitalWrite(LED,HIGH)



