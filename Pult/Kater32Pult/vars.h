#define TFT_DC PB1
#define TFT_CS PB0

stControl ctrl; // управление с пульта
stTelemetry tlm, old; // телеметрия с катера

EasyTransfer Pult; 
EasyTransfer Kater;
 
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

#define LED PC13
#define LED_ON digitalWrite(LED,LOW)
#define LED_OFF digitalWrite(LED,HIGH)

#define POINT_NUM 10
uint8_t point_idx=0; // номер текущей точки
stPoint points[POINT_NUM]; // массив точек

#define RUL_PIN PA0
#define GAZ_PIN PA1

uint16_t adc_rul, adc_gaz;

//#define SER Serial3 //B10
//#define SER Serial2 //A2
//#define SER Serial1 //A9

