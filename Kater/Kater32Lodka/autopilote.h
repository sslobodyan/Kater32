#define PID_KP 1.0
#define PID_KI 2.0

float src_lat, src_lon, dest_lat, dest_lon;

void init_pid() {
  //pid_kp = (float) PID_KP * ctrl.pid / 128.0;
  //pid_kd = (float) PID_KI * ctrl.pid / 128.0;
  
  //pid_kp = (float) flash.pid_kp * ctrl.pid / 1280.0;
  //pid_kd = (float) flash.pid_kd * ctrl.pid / 1280.0;
  
  pid_kp = (float) flash.pid_kp / 1000.0;
  pid_ki = (float) flash.pid_ki / 1000.0;
  pid_kd = (float) flash.pid_kd / 1000.0;
  min_pi = (float) flash.min_pi / 1000.0;
  max_pi = (float) flash.max_pi / 1000.0;

  old_res_i = 0; // сбрасываем интегратор
  old_pid_error = 0; // сбрасываем дифференциатор
  
  DBG.print("PID Kp=");DBG.println(pid_kp,6);
  DBG.print("PID Ki=");DBG.println(pid_ki,6);
  DBG.print("PID Kd=");DBG.println(pid_kd,6);
}

bool is_in_auto() {
  bool ret = false;
  if ( (dest_lat != BAD_GRD) && (dest_lon != BAD_GRD ) ) {
    ret = true;
  }
  return ret;
}

void set_destination(stPoint pnt) {
  dest_lat = (float) pnt.lat /  INT2FLOAT_MUX;
  dest_lon = (float) pnt.lon /  INT2FLOAT_MUX;
}

void set_source(stPoint pnt) {
  src_lat = (float) pnt.lat /  INT2FLOAT_MUX;
  src_lon = (float) pnt.lon /  INT2FLOAT_MUX;
}

void autopilote_on( stPoint pnt ) {
  if ( is_in_auto() ) return;
  init_pid();
  set_destination( pnt );
  tlm.gps.sat.autopilot = true;
  DBG.print(F("autopilote_on"));
  DBG.print(" Destination: Lat "); DBG.print(pnt.lat); DBG.print("  Lon "); DBG.println( pnt.lon );
}

void autopilote_off() {
  if ( !is_in_auto() ) return;
  stPoint pnt;
  pnt.lat = BAD_POINT;
  pnt.lon = BAD_POINT;
  set_destination( pnt );
  tlm.gps.sat.autopilot = false;
  DBG.println(F("autopilote_off"));
}

int8_t pid_corr(float est, float nado) {
  float res_p, res_i, res_d;
  float pid_error;
  float res;

  //kp=1.0;
  //ki=2.0*kp;
  //kd=0.125*kp;

  pid_error = nado-est; 
  // приводим градус ошибки к +-180 !!! VERY IMPORTMANT !!!!
  while (pid_error < -180) pid_error+=360;
  while (pid_error > 180) pid_error-=360;

  // на больших углах тупо руль до упора
  if (pid_error > 60) return 100;
  if (pid_error < -60) return -100;

  res_p = (float) pid_kp * pid_error; // пропорционально ошибке
  
  res_i = old_res_i + pid_ki * pid_error; // накопитель отклонения (интегратор)
  if (res_i > max_pi) res_i = max_pi;
  if (res_i < min_pi) res_i = min_pi;
  
  res_d = (float) pid_kd*(pid_error-old_pid_error); // скорость изменения ошибки (дифференциатор)
  
  //res = (float) pid_kp*pid_error + pid_kd*(pid_error-old_pid_error);
  res = res_p + res_i + res_d;
  old_pid_error = pid_error;
  old_res_i = res_i; // накапливаем интегратор
  if (res > 100) res = 100;
  if (res < -100) res = -100;
  if (show_pid) {
    DBG.print("PID (");DBG.print(est);DBG.print(",");DBG.print(nado);DBG.print(")=");DBG.println(res);
  }
  return res;
}

bool update_autopilote() {
  int8_t old_corr_autopilote;
  if ( !is_in_auto() ) {
    corr_autopilote = 0;
    return false;
  } else {
    float lat = (float) posllh.lat / INT2FLOAT_MUX;
    float lon = (float) posllh.lon / INT2FLOAT_MUX;

    if (rul > 30 || rul < -30) { // подергали рулем
      DBG.println("Change RUL");
      corr_autopilote = 0;
      autopilote_off();
      return false;    
    }
    if (gaz > 30 || gaz < -30) { // подергали газом
      DBG.println("Change GAZ");
      corr_autopilote = 0;
      autopilote_off();
      return false;    
    }
    
    float heading = GetHeading( lat, lon, dest_lat, dest_lon );
    old_corr_autopilote = corr_autopilote;
    corr_autopilote = pid_corr(kurs_gps, heading);

    to_point = GetDistanceInM( lat, lon, dest_lat, dest_lon );
    if (to_point < diam) {
      DBG.print("In WP! "); DBG.println( to_point );
      autopilote_off();
      return false;
    } 
    else if (to_point > 500.0) {
      DBG.print("Wrong Distanse "); DBG.println( to_point );
      autopilote_off();
      return false;
    } else {
      if ( false && (corr_autopilote != old_corr_autopilote) ) {
        DBG.print("Now=");DBG.print(lat,7);DBG.print(",");DBG.print(lon,7);
        DBG.print("  Pnt=");DBG.print(dest_lat,7);DBG.print(",");DBG.print(dest_lon,7);
        
        DBG.print(" Kurs=");DBG.print(kurs_gps);
        DBG.print(" Heading=");DBG.print(heading);
        DBG.print(" Distanse=");DBG.print(to_point);
        DBG.print(" CorrO=");DBG.println(corr_autopilote);  
      }
      return true;
    }
  }
}

