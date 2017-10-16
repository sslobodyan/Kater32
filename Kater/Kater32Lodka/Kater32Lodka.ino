#include "arduino.h"
#include "Wire.h"
//#include <HardWire.h>


#define USE_DEBUG_UBLOX_

#include <EasyTransfer.h>
EasyTransfer Pult; 
EasyTransfer Kater;

#define HC12 Serial2 // a2-TX a3-RX
#define DBG Serial3 // b11 b10

#include "radio.h"
#include "vars.h"
#include "ublox.h"
#include "navigate.h"
#include "autopilote.h"
#include "workers.h"
#include "sonar.h"
#include "imu.h"
#include "test.h"
#include "sensors.h"

#define INTERVAL 500


void testI2C() {
  //i2c_init(I2C1);
  i2c_master_enable(I2C1,  I2C_BUS_RESET );//I2C_FAST_MODE

  //i2c_start_condition(I2C1);
  //i2c_stop_condition(I2C1);
  
  //  I2Cread(MPU9250_ADDRESS, 117, 1, &id);
/*   
  Wire.beginTransmission(Address);
  Wire.write(Register);
  Wire.endTransmission();

  // Read Nbytes
  Wire.requestFrom(Address, Nbytes);
  uint8_t index = 0;
  while (Wire.available())
    Data[index++] = Wire.read(); 
 */
  uint8_t mas[] = {1};
  i2c_msg mess;
  mess.addr = 0x68;
  mess.flags = I2C_MSG_READ | I2C_MSG_10BIT_ADDR;
  mess.length = 1;
  mess.xferred = 0;
  mess.data = &mas[0];
  i2c_master_xfer(I2C1,  &mess,  1,  0);
  DBG.print("I2C=0x");
  DBG.println(mas[0],HEX);
}


void setup() {

  delay(500);

  HC12.begin(38400);
  HC12.println("CW32");
  HC12.flush();
  
  DBG.begin(115200);
  DBG.println(F("CarpWinner 32 RX"));
  DBG.flush();

  setup_workers();

  tlm.kurs = 255;

  //testI2C();

  Pult.begin(details(ctrl), &HC12);
  Pult.set_mask( (uint8_t) 'C', (uint8_t) 'w' );
  Kater.begin(details(tlm), &HC12);
  Kater.set_mask( (uint8_t) 'C', (uint8_t) 'w' );

  pinMode(LED, OUTPUT);
  tm = millis();  


  sonar_setup();

/*
  Compass.begin();
  for (byte nn=0; nn<15; nn++) {
    Compass.update();
  }
  //start_heading = Compass.tilledHeading();
*/

  LED_ON;

  LED4_ON;
  ublox_setup();
  home_pnt.lat = BAD_POINT;
  home_pnt.lon = BAD_POINT;
  LED4_OFF;

  tm_sensors = millis() + 2000;

  init_vars();

  LED_OFF;
  LED1_OFF;
  LED4_OFF;
  DBG.println("Setup DONE\n");
  DBG.flush();
  
}


void loop() {
  if ( millis() > tm ) {
    LED_OFF;
  }

  if ( millis() > tm_gps ) {
    tlm.gps.sat.present = false;
  }

  if ( millis() > tm_sensors ) {
    if (tm_sensors > 0) {
      init_sensors();
      tm_sensors = 0;
    } else {
      update_sensors();      
    }
  }

  update_gps();

  if(Pult.receiveData()){ // обработка принятого пакета
      LED_ON;
      tm = millis() + INTERVAL;
      update_workers();
      // отправляем ответ
      // test_data(); // TODO DEBUG
      //test_sonar(); // TODO DEBUG
    
      tlm.sonar.delta = ctrl.sonar.delta;
      tlm.sonar.speed = ctrl.sonar.speed;
      tlm.sonar.treshold = ctrl.sonar.treshold;
      if (ctrl.home.lat < BAD_POINT) { // приняли координаты домашней 0 точки
        home_pnt = ctrl.home;
      }

      Kater.sendData();
  }

  if ( sonar_newdata  ) { // принялось эхо
    sonar_newdata=false;
    
    sonar_update(); // 165 mks обрабатываем принятое эхо

    attachInterrupt(SONAR_INT_PIN, ExternSonarInt, RISING); // ставим сторожа на новое эхо
  }

/*
  uint32_t t = millis();
  Compass.update(); // 10 ms ???
  tlm.tok = (millis()-t) ; // 
  
  byte heading_base = Compass.tilledHeading(); // 21 mks
  tlm.kurs = heading_base / 2;
*/

 
}
