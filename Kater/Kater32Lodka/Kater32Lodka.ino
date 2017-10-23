#include "arduino.h"
#include <EEPROM.h>
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
#include "eeprom.h"
#include "ublox.h"
#include "navigate.h"
#include "autopilote.h"
#include "workers.h"
#include "sonar.h"
#include "imu.h"
#include "test.h"
#include "sensors.h"
#include "console.h"


#define INTERVAL 500
#define INTERVAL_WORKERS 100

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

  setup_eeprom();
  read_from_eprom();
  if ((flash.carp != 'C') || (flash.winner != 'W')) { // флеш не инициализирована
    init_flash();
    read_from_eprom();
  } else {
    flash.cnt += 1;
    save_to_eprom( EEPROM_CNT );
  }

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
  if ( millis() > tm ) { // нет связи
    LED_OFF;
    if (tm_no_radio == 0) {
      tm_no_radio = millis();
    }
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

  if ((tm_no_radio > 0) && (millis() - tm_no_radio > 1000)) { // стопмашина при кратковременной потере связи
      if (cnt_no_radio++ > 10) {
        if (autohome) {
          DBG.println("Goto HOME!");
          autopilote_on( ctrl.home );
          cnt_no_radio = 0;
        }        
      }
      DBG.print("Stop! ");
      DBG.println(cnt_no_radio);
      tm_no_radio = millis();
      ctrl.rul = 0;
      ctrl.gaz = 0;
  }
  
  update_gps();

  if (millis() > tm_workers) {
    tm_workers = millis() + INTERVAL_WORKERS;
    update_workers();
  }

  if(Pult.receiveData()){ // обработка принятого пакета
      LED_ON;
      tm_no_radio = 0;
      cnt_no_radio = 0;
      tm = millis() + INTERVAL;
    
      tlm.sonar.delta = ctrl.sonar.delta;
      tlm.sonar.speed = ctrl.sonar.speed;
      tlm.sonar.treshold = ctrl.sonar.treshold;
      if (ctrl.home.lat < BAD_POINT) { // приняли координаты домашней 0 точки
        home_pnt = ctrl.home;
      }
      tlm.sonar.cnt++; // TODO пока считаем каждую посылку как новое эхо
      Kater.sendData();
  }

  if ( sonar_newdata  ) { // принялось эхо
    sonar_newdata=false;
    
    sonar_update(); // 165 mks обрабатываем принятое эхо

    attachInterrupt(SONAR_INT_PIN, ExternSonarInt, RISING); // ставим сторожа на новое эхо
  }

  update_bt_command();
  
/*
  uint32_t t = millis();
  Compass.update(); // 10 ms ???
  tlm.tok = (millis()-t) ; // 
  
  byte heading_base = Compass.tilledHeading(); // 21 mks
  tlm.kurs = heading_base / 2;
*/
  //DBG.println("---");
 
}
