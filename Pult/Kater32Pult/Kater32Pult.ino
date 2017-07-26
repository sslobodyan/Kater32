#include "arduino.h"
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <EasyTransfer.h>

#define HC12 Serial2
#define DBG Serial1 // a9

#include "radio.h"
#include "vars.h"
#include "screen.h"
#include "libs.h"



uint32_t tm, tm_loop;
#define INTERVAL 80

void scan_controls();
void send_paket();
void init_vars();

void setup() {

  screen_setup();
  
  HC12.begin(38400);
  DBG.begin(115200);
  Pult.begin(details(ctrl), &HC12);
  Kater.begin(details(tlm), &HC12);

  pinMode(LED, OUTPUT);
  LED_ON;
  tm, tm_loop = millis();  

  init_vars();
  update_screen(true); // refresh statik
  
}


void loop() {

  if ( millis() - tm > 500 ) { // гасим светодиод принятого пакета
    LED_OFF;
  }

  if (HC12.available()) {
    //LED_ON; tm = millis();    
  }

  if(Kater.receiveData()){ // обработка принятого пакета
    LED_ON;
    tm = millis();
  }

  if ( millis() > tm_loop ) { // прошло 80 мс
    tm_loop = millis() + INTERVAL;
    
    scan_controls();

    send_paket();
    
    update_screen();
  }
  
}


void init_vars() {
  old.kurs = 99;
  old.gps.sat.cnt = 1;
  old.gps.sat.speed = 9;
  old.bort = 1;
  old.tok = 1;
  old.sonar.delta = 3;
  tlm.sonar.speed=0;
  tlm.bort=114;
  tlm.tok=39;
  tlm.kurs=45;
  tlm.sonar.cnt=2;
  tlm.sonar.treshold = 59;
}

