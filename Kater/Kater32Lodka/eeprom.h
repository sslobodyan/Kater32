#define START_EPROM_ADDR 0


struct stFlash { // надо выровнять на 2 байта 
  //0
  char carp; // сигнатура правильного епрома
  char winner;
  //1
  uint16_t cnt; // количество перезапусков
  //2
  int16_t pid_kp;
  //3 
  int16_t pid_ki;
  //4 
  int16_t pid_kd;
  //5
  uint8_t sonar_koeff;
  uint8_t treshold_dno;
  //6
  uint8_t gaz;
  uint8_t rezerv;
  //7 
  int16_t min_pi;
  //8 
  int16_t max_pi;
} flash;

#define EEPROM_CNT 1
#define EEPROM_PIDP 2
#define EEPROM_PIDI 3
#define EEPROM_PIDD 4
#define EEPROM_SONAR_DNO 5
#define EEPROM_GAZ 6
#define EEPROM_MINPI 7
#define EEPROM_MAXPI 8



uint16_t setup_eeprom(void) {
  EEPROM.PageSize = 0x400; // 1024
  EEPROM.PageBase0 = 0x8000000 + 126*EEPROM.PageSize;    //0x801F800;
  EEPROM.PageBase1 = 0x8000000 + 127*EEPROM.PageSize; // 134217728 + 125*1024 = 134345728 0x801F400
  return ( EEPROM.init() );
}

uint16_t write_eeprom(uint16_t AddressWrite, uint16_t DataWrite) { // записать 16 бит
  return ( EEPROM.write(AddressWrite, DataWrite) );
}

uint16_t read_eeprom(uint16_t AddressWrite ) { // считать 16 бит
  uint16_t Data;
  EEPROM.read(AddressWrite, &Data);
  return ( Data );
}

uint16_t save_to_eprom(void) { // записать из структуры flash
  uint16_t addr = START_EPROM_ADDR;
  uint16_t data;
  uint16_t* ptr;
  uint16_t stat;
  flash.cnt += 1;
  ptr = (uint16_t*) &flash;
  for (int i = 0; i < sizeof(flash) / sizeof(int16_t); i++) {
    data = *ptr;
    stat = write_eeprom( addr++, data);
    ptr++;
  }
  return stat;
}

uint16_t save_to_eprom(uint16_t addr) { // записать из структуры flash одну переменную
  uint16_t data;
  uint16_t* ptr;
  ptr = (uint16_t*) &flash + addr;
  data = *ptr;
  return (write_eeprom( addr++, data));
}


void read_from_eprom(void) { // считать в структуру flash
  uint16_t addr = START_EPROM_ADDR;
  uint16_t data;
  uint16_t* ptr;
  ptr = (uint16_t*) (&flash);
  for (int i = 0; i < sizeof(flash) / 2; i++) {
    data = read_eeprom( addr++ );
    *(ptr++) = data;
  }
}

void read_from_eprom(uint16_t addr) { // считать в структуру flash одну переменную
  uint16_t data;
  uint16_t* ptr;
  ptr = (uint16_t*) (&flash);
  data = read_eeprom( addr );
  *(ptr) = data;
}


void init_flash(void) {
  EEPROM.format();
  flash.carp = 'C';
  flash.winner = 'W';
  flash.cnt = 0;
  flash.pid_kp = 100; // в сотых 100 == 1.0f
  flash.pid_kd = 200;
  flash.gaz = 60;
  flash.sonar_koeff = 40;
  save_to_eprom();
}



