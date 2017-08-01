#define LED PC13
#define LED_ON digitalWrite(LED,LOW)
#define LED_OFF digitalWrite(LED,HIGH)
#define LED_TOGGLE digitalWrite(LED,!digitalRead(LED))



struct stSonarControl { // 2
  uint8_t delta:6;
  uint8_t speed:1;
  uint8_t treshold;
};

struct stLight { // 1
  uint8_t light:2;
  uint8_t bunker:2;  
};

struct stPoint { // 8
  uint32_t lat;
  uint32_t lon;
};

struct stSat { // 3
  uint8_t speed; // 1
  uint8_t cnt:6; // 1
  uint8_t fix:1;
  uint8_t present:1;
  uint8_t second; // 1
};

struct stSonar { //64
  uint8_t map[60];
  uint8_t deep; // глубина / 10 (прибавить еще delta для реального результата до 25.5+32)
  uint8_t level; // уровень сигнала на дне
  uint8_t treshold;
  uint8_t delta:5; // сдвиг окна в целых метрах
  uint8_t speed:1; // скорость сканирования
  uint8_t cnt:2; // счетчик номера сканирования
};

struct stGps { // 12 
  stPoint coord; //8
  stSat sat; //3
};


////////////////////////// передается на катер //////////////////
struct stControl { // 24
  int8_t rul;
  int8_t gaz;
  stLight light; //1 
  stSonarControl sonar; //2
  stPoint point; // 8
  stPoint home; // 8
}; 
/////////////////////////////////////////////////////////////////

/////// приходит с катера ////////////////////////
struct stTelemetry { // 80 
  uint8_t bort; // 1
  uint8_t tok;  // 1
  uint8_t kurs; // 1
  uint8_t rezerv; // может состояние света и кузова ?
  stGps gps; // 12
  stSonar sonar; // 64
};
//////////////////////////////////////////////////


