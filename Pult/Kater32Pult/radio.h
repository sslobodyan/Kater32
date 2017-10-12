struct stSonarControl { // 2
  uint8_t delta:6;
  uint8_t speed:1;
  uint8_t rezerv:1;
  uint8_t treshold;
};

struct stLight { // 1
  uint8_t light:3;
  uint8_t bunker:1;  
  uint8_t rezerv:4;
};

struct stPoint { // 8
  uint32_t lat;
  uint32_t lon;
};

struct stSat { // 3
  uint8_t speed; // 1 скорость
  uint8_t cnt:6; // 1 количество спутников
  uint8_t fix:1; // 3Д фикс
  uint8_t present:1; // принят хотя бы один нормальный кадр от ГПС приемника
  uint8_t second:6; // текущая секунда
  uint8_t autopilot:1; // идем по автопилоту
  uint8_t bunker:1; // бункер открыт
};

struct stSonar { //64 приходит с катера
  uint8_t map[60];
  uint8_t deep; // глубина / 10 (прибавить еще delta для реального результата до 25.5+32)
  uint8_t level; // уровень сигнала на дне
  uint8_t treshold; // ??
  uint8_t delta:5; // сдвиг окна в целых метрах ??
  uint8_t speed:1; // скорость сканирования ??
  uint8_t cnt:2; // счетчик номера сканирования
};

struct stGps { // 12 
  stPoint coord; //8
  stSat sat; //3
};


////////////////////////// передается на катер //////////////////
struct stControl { // 23
  int8_t rul;
  int8_t gaz;
  int8_t trim;
  uint8_t pid;
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


