#include "arduino.h"

#include <EasyTransfer.h>
EasyTransfer Pult; 
EasyTransfer Kater;

#include "radio.h"
#include "vars.h"
#include "workers.h"

#define HC12 Serial2
#define DBG Serial1 // a9

#define LED PC13
#define LED_ON digitalWrite(LED,LOW)
#define LED_OFF digitalWrite(LED,HIGH)

uint32_t tm;
#define INTERVAL 500

void update_workers();

void setup() {
  HC12.begin(38400);
  DBG.begin(115200);

  Pult.begin(details(ctrl), &HC12);
  Kater.begin(details(tlm), &HC12);

  pinMode(LED, OUTPUT);
  LED_ON;
  tm = millis();  

  setup_workers();
}


uint8_t calc_deep( uint8_t idx ) {
   return (float) idx * 0.435 * (ctrl.sonar.speed+1); 
}

static int8_t sonar=60;

void test_data() {
  uint8_t eho[] = {9,13,15,14,15,13,13,14,11,10,7,5,7,11,12,7,5,3,1,1,2,2,1,1};
  uint8_t buf[120];
  
  tlm.kurs += 1;
  tlm.bort += 1;
  tlm.gps.sat.speed += 1;
  tlm.gps.sat.cnt = 15;
  tlm.gps.sat.fix = 1;
  tlm.gps.sat.second += 1;

  sonar += (random(5)-2);
  if (sonar >= 110) sonar = 109;
  if (sonar < 20) sonar = 20;

  tlm.sonar.deep = calc_deep( sonar );

  byte i=0;
  while (i<sonar) {
    buf[i] = 0;
    i++;
  }

  byte n=0;
  while ( n<sizeof(eho) ) {
    buf[i] = eho[n];
    i++;
    if (i==120) break;
    n++;
  }

  while ( i < 120 ) {
    buf[i] = 0;
    i++;
  }

  for (i=0; i<120; i+=2) {
    tlm.sonar.map[i/2] = (buf[i] << 4) + buf[i+1];
  }
  
  tlm.sonar.cnt += 1;
}

void loop() {
  if ( millis() > tm ) {
    LED_OFF;
  }

  if(Pult.receiveData()){ // обработка принятого пакета
      LED_ON;
      tm = millis() + INTERVAL;
      update_workers();
      // отправляем ответ
      test_data();
      Kater.sendData();
      //HC12.println("SEND");
  }
  
}
