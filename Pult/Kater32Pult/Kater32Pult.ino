#include "arduino.h"
#include <EEPROM.h>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <EasyTransfer.h>

#define HC12 Serial2
#define DBG Serial1 // a9

#include "radio.h"
#include "vars.h"
#include "eprom.h"
#include "screen.h"
#include "libs.h" 
#include "menu.h" 
#include "keys.h" 


#define INTERVAL 80

void scan_controls();
void send_paket();
void init_vars();

void setup() {

  screen_setup();
  
  HC12.begin(38400);
  HC12.println("CarpWinner");
  HC12.flush();
  
  DBG.begin(115200); // не исп
  setup_eeprom();
  read_from_eprom();
  if ((flash.carp != 'C') || (flash.winner != 'W')) { // флеш не инициализирована
    init_flash();
    read_from_eprom();
  } else {
    flash.cnt += 1;
    save_to_eprom( EEPROM_CNT );
  }

  Pult.begin(details(ctrl), &HC12);
  Pult.set_mask( (uint8_t) 'C', (uint8_t) 'w' );
  Kater.begin(details(tlm), &HC12);
  Kater.set_mask( (uint8_t) 'C', (uint8_t) 'w' );

  pinMode(LED, OUTPUT);
  LED_ON;
  tm, tm_loop = millis();  

  init_vars();
  update_screen(true); // refresh statik
  screen_hello();
}


void loop() {

  if ( millis() - tm > 500 ) { // гасим светодиод принятого пакета
    LED_OFF;
  }
  
  if ( millis() > tm_bunker ) { // опускаем бункер
    ctrl.light.bunker = 0;
  }

  if ( millis() > tm_auto ) { // передали уже точку для автопилота
    autopilot_off();
  }  

  if(Kater.receiveData()){ // обработка принятого пакета
    //for (byte i=0; i<40; i++) { DBG.print( tlm.sonar.map[i],HEX ); DBG.print( "," ); } DBG.println();
    LED_ON;
    tm = millis();
    //update_test( tlm.sonar.treshold );
    update_test( tlm.kurs*2 );
  }

  if ( millis() > tm_loop ) { // прошло 80 мс
    tm_loop = millis() + INTERVAL;
    
    scan_controls();

    send_paket();
    
    update_key();
    
    update_screen();
  }
  
}


void init_vars() {
  ctrl.sonar.speed=flash.sonar_speed;
  tlm.sonar.speed=0;
  old.sonar.speed=100;

  ctrl.sonar.delta=flash.sonar_delta;
  tlm.sonar.delta=0;
  old.sonar.delta=99;

  ctrl.sonar.treshold = flash.sonar_treshold;
  tlm.sonar.treshold = 0;
  old.sonar.treshold = 11;
  
  old.sonar.cnt = tlm.sonar.cnt;

  tlm.sonar.deep=0;
  old.sonar.deep=1;
  
  tlm.bort=1;
  old.bort = 1;
  
  tlm.tok=1;
  old.tok = 1;

  tlm.kurs=45;
  old.kurs = 45;

  tlm.gps.coord.lat = BAD_POINT;
  tlm.gps.coord.lon = BAD_POINT;
}

