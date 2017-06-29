
struct stGps { // 12 bytes
  uint32_t lat=491234567; //4
  uint32_t lon=211234567; //4 
  uint8_t sat_speed=12; //1
  uint8_t sat_cnt:5; //1
  uint8_t sat_fix:1;
  uint8_t light:2;
  uint8_t kurs=45; //1 по 2 градуса\отсчет (0-180)
  uint8_t idx:4;  // 1   16 точек
  uint8_t mode:1; // 0-ручной, 1-автопилот
  uint8_t fill:1; // 0=пустая точка, 1-точка с координатами
  uint8_t bunker:2;   // статус двух бункеров 0-закрыт, 1-открыт
};

struct stRadio { //4+12+32=48bytes
  stGps gps;
  uint8_t ubort=125; //1
  uint8_t ibort=46; //1
  uint16_t id; //2 идентификатор получателя - подхватывается при спаривании
  uint8_t sonar_speed:1;  //1   0-9м, 1-18м
  uint8_t sonar_delta:5;  // 32м смещение в глубину
  uint8_t sonar_cnt:2;  // счетчик, перебрасывается при каждом новом эхе
  uint8_t sonar_deep; //1 глубина по эху без учета дельты
  uint8_t sonar_data[30]; //30
  uint8_t sonar_treshold; // 1 чисто для индикатора чувствительности
  uint8_t sonar_level; // уровень эха на дне
  uint8_t mode; // режим - какую переменную меняем
  uint8_t debug;
}; 

