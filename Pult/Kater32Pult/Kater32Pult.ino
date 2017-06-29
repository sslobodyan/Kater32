#include "arduino.h"
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <EasyTransfer.h>

#define HC12 Serial2

#include "radio.h"
#include "vars.h"
#include "screen.h"


uint32_t tm;
#define INTERVAL 200

void set_init();

void setup() {
  byte i = sizeof(radio);  

  screen_setup();
  
  HC12.begin(9600);
  ET.begin(details(radio), &HC12);

  pinMode(LED, OUTPUT);
  LED_ON;
  tm = millis();  
  
  set_init();
  
  update_screen(true);
  
  old.ubort = 2;
  old.ibort = 2;
}

uint32_t tm_test = millis();

void loop() {
  if(ET.receiveData()){
    // обработка принятого пакета
    LED_ON;
    tm = millis();
    update_screen();
  }

  if ( millis() - tm > INTERVAL ) {
    tm = 0;
    LED_OFF;
  }

  if ( millis() > tm_test ) {
    radio.sonar_cnt += 1;
    radio.sonar_deep += 1; 
    radio.gps.kurs += 1;
    if (radio.gps.kurs > 179) radio.gps.kurs = 0;
    tm_test = millis() + 200; 
    update_screen();
  }
  
}

void set_init(){
  old.ubort = 2;
  old.ibort = 2;  
  old.sonar_speed = 0;  
  old.sonar_delta = 2;  
  old.sonar_cnt = 1;
  old.sonar_deep = 12;  
  old.gps.sat_cnt = 8;  
  old.gps.sat_fix = 1;  
  old.gps.kurs = 125;  
  old.gps.idx = 1;  
  old.gps.fill = 1;  
  old.gps.bunker  = 0b11;  
  old.sonar_treshold = 35;
  radio.sonar_treshold = 43;
  radio.sonar_deep = 123;
  radio.sonar_delta = 2;
  radio.sonar_speed = 1;
  radio.gps.idx = 14;
  radio.gps.kurs = 7;
}

