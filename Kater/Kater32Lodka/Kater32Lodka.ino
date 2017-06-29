#include "arduino.h"
#include "radio.h"

#include <EasyTransfer.h>
EasyTransfer ET; 

#define HC12 Serial2
stRadio radio;

#define LED PC13
#define LED_ON digitalWrite(LED,LOW)
#define LED_OFF digitalWrite(LED,HIGH)

uint32_t tm;
#define INTERVAL 50

void setup() {
  HC12.begin(9600);
  ET.begin(details(radio), &HC12);

  pinMode(LED, OUTPUT);
  LED_ON;
  tm = millis();  
}

void loop() {
  if ( millis() - tm > INTERVAL ) {
    radio.sonar_cnt += 1;
    radio.sonar_data[0] += 1;
    ET.sendData();
    tm = millis();
    if (radio.sonar_cnt) LED_ON;
    else LED_OFF;
  }
  
}
