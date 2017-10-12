#define START_EPROM_ADDR 0

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
  flash.points[0].lat = 494358565;
  flash.points[0].lon = 271025416;
  for(byte i=1; i<POINT_NUM; i++) {
    flash.points[i].lat = BAD_POINT;
    flash.points[i].lon = BAD_POINT;
  }
  flash.sonar_treshold = 10;
  flash.sonar_delta = 0;
  flash.sonar_speed = 0;
  flash.trim_rul = 0;
  flash.carp = 'C';
  flash.winner = 'W';
  flash.cnt = 0;
  save_to_eprom();
}



