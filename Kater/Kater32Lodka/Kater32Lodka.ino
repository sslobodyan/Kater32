#include "arduino.h"
#include "radio.h"

#include <EasyTransfer.h>
EasyTransfer Pult; 
EasyTransfer Kater;

#define HC12 Serial2
stControl ctrl; // управление с пульта
stTelemetry tlm; // телеметрия с катера

#define LED PC13
#define LED_ON digitalWrite(LED,LOW)
#define LED_OFF digitalWrite(LED,HIGH)

uint32_t tm;
#define INTERVAL 500

void setup() {
  HC12.begin(38400);
  Pult.begin(details(ctrl), &HC12);
  Kater.begin(details(tlm), &HC12);

  pinMode(LED, OUTPUT);
  LED_ON;
  tm = millis();  
}


void test_data() {
  tlm.kurs += 1;
  tlm.bort += 1;
  tlm.gps.sat.speed += 1;
  for (byte i=0; i<60; i++ ) {
    tlm.sonar.map[i] = random(128);
    //tlm.sonar.map[i] = i;
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
      // отправляем ответ
      test_data();
      Kater.sendData();
      //HC12.println("SEND");
  }
  
}
