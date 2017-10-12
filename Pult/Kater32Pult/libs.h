

void scan_controls() {
    adc_rul = analogRead(RUL_PIN); // 8 mks один замер
    adc_gaz = analogRead(GAZ_PIN);        
}


void send_paket() {
  if ( is_menu ) { // в меню считаем джойстик находится по центру
    ctrl.rul = 0;
    ctrl.gaz = 0;
  } else {
    ctrl.rul = map(adc_rul, 0, 4096, -127, 128);
    ctrl.gaz = map(adc_gaz, 0, 4096, -127, 128);   
  }
  //DBG.print(" sat ");DBG.print(tlm.gps.sat.cnt);
  //DBG.print(" rul ");DBG.print(ctrl.rul);DBG.print(" gaz ");DBG.println(ctrl.gaz);
  ctrl.trim = flash.trim_rul;
  Pult.sendData();
}

String utf8rus(String source)
{
  int i, k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };

  k = source.length(); i = 0;

  while (i < k) {
    n = source[i]; i++;

    if (n >= 0xC0) {
      switch (n) {
        case 0xD0: {
            n = source[i]; i++;
            if (n == 0x81) {
              n = 0xA8;
              break;
            }
            if (n >= 0x90 && n <= 0xBF) n = n + 0x30;
            break;
          }
        case 0xD1: {
            n = source[i]; i++;
            if (n == 0x91) {
              n = 0xB8;
              break;
            }
            if (n >= 0x80 && n <= 0x8F) n = n + 0x70;
            break;
          }
      }
    }
    m[0] = n; target = target + String(m);
  }
  return target;
}

bool is_point_fill( stPoint point ) {
  bool res = true;
  if ( point.lat == BAD_POINT ) res = false; 
  if ( point.lon == BAD_POINT ) res = false;
  return res;
}

void open_bunker() {
    if ( ctrl.light.bunker  ) return;
    if ( (!is_point_fill( flash.points[point_idx]) && (is_point_fill( tlm.gps.coord )) ) ) {
      flash.points[point_idx].lat = tlm.gps.coord.lat;  
      flash.points[point_idx].lon = tlm.gps.coord.lon;  
      save_to_eprom();
      if ( point_idx==0 ) { // запомним домашнюю точку
        ctrl.home.lat = tlm.gps.coord.lat;  
        ctrl.home.lon = tlm.gps.coord.lon;
      }
      if (point_idx < POINT_NUM-1 ) point_idx++;
    }
    ctrl.light.bunker = 1;
    tm_bunker = millis() + TIME_BUNKER_OPEN;
    old_point_idx = 99;
}

void shift_sonar_buffer(bool depper){
  uint8_t pixels_in_metr = PIXEL_IN_M_1/2;
  for (uint16_t i=0; i<BUF_CNT-1; i++) {
    if (depper) {
      for (int8_t c=59; c>0; c--) {
        if (c-pixels_in_metr>-1) buf[i][c] = buf[i][c-pixels_in_metr];
        else buf[i][c] = 0;
      }
    } else {
      for (uint8_t c=0; c<60; c++) {
        if (c+pixels_in_metr < 60) buf[i][c] = buf[i][c+pixels_in_metr];
        else buf[i][c] = 0;
      }
    }
  }
}

void autopilot_on(){
  // в течении 1 секунды передаем точку назначения
  tm_auto = millis() + TIME_SEND_POINT;
  ctrl.point.lat = flash.points[point_idx].lat;
  ctrl.point.lon = flash.points[point_idx].lon;
}

void autopilot_off(){
  ctrl.point.lat = BAD_POINT;
  ctrl.point.lon = BAD_POINT;  
}

