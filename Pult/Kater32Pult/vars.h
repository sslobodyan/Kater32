#define REMAP_SPI1

#ifdef REMAP_SPI1
  #define TFT_DC PA12
  #define TFT_CS PA15
  #define RUL_PIN PB1
  #define GAZ_PIN PB0
  
#else
  #define TFT_DC PB1
  #define TFT_CS PB0
  #define RUL_PIN PA0
  #define GAZ_PIN PA1
#endif


stControl ctrl; // управление с пульта
stTelemetry tlm, old; // телеметрия с катера
uint8_t old_point_idx=99;
uint8_t auto_point = 99; // номер точки автопилота

EasyTransfer Pult; 
EasyTransfer Kater;
 
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

#define LED PC13
#define LED_ON digitalWrite(LED,LOW)
#define LED_OFF digitalWrite(LED,HIGH)

#define POINT_NUM 10
uint8_t point_idx=0; // номер текущей точки
//stPoint points[POINT_NUM]; // массив точек - перенес во флеш

#define XSONAR 90
#define WSONAR 194
#define YSONAR 0
#define HSONAR 240

#define BUF_CNT WSONAR
uint8_t buf[BUF_CNT][60]; // карта эхолота
bool menu_sonar = true; // показывать в меню настройки сонара и калибровки
bool is_left;

uint32_t tm, tm_loop, tm_bunker, tm_auto;

#define BUTTON_CNT_CLICK 2
#define BUTTON_CNT_PRESS 6
int button_cnt=0;

#define TIME_BUNKER_OPEN 5000 // время открытия бункера
#define TIME_SEND_POINT 1000 // время передачи точки автопилоту
#define TIME_TO_NOTHING_RESEIVE 1000 // через сколько миллисекунд понимаем что сигнал потерян

uint16_t adc_rul, adc_gaz;

bool is_menu = false; // перешли в режим меню и джойстик теперь управляет менюшками
bool refresh_menu=false;
byte id_menu=0;
bool first_refresh_menu=true;

void update_menu_screen_sonar();
void update_menu_screen_kalibr();
void show_menu1();
void open_bunker();
bool is_point_fill( stPoint point, bool checkdistanse );
void update_test(uint16_t data);

//#define SER Serial3 //B10
//#define SER Serial2 //A2
//#define SER Serial1 //A9

// правильные координаты
#define INT2FLOAT_MUX 10000000
#define BAD_GRD 200.0
#define BAD_POINT BAD_GRD * INT2FLOAT_MUX


struct stFlash { // надо выровнять на 2 байта == 88 байта
  //0
  char carp; // сигнатура правильного епрома
  char winner;
  //1
  uint16_t cnt; // количество перезапусков
  //2
  uint8_t sonar_delta;
  uint8_t sonar_speed;
  //3
  uint8_t sonar_treshold; 
  int8_t trim_rul; 
  //4-43 по 4
  stPoint points[POINT_NUM]; // точки для автопилота 10 * 8 = 80 байт 
  //44 
  uint8_t pid;
  uint8_t rezerv;
} flash;

#define EEPROM_CNT 1
#define EEPROM_DELTA_SPEED 2
#define EEPROM_TRESHOLD_TRIM 3
#define EEPROM_POINT0 4
#define EEPROM_PID 44

