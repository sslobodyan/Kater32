
float src_lat, src_lon, dest_lat, dest_lon;

void init_pid() {
  pid_kp = (float) PID_KP * ctrl.pid / 128.0;
  pid_ki = (float) PID_KI * ctrl.pid / 128.0;
  DBG.print("PID Kp=");DBG.println(pid_kp,6);
  DBG.print("PID Ki=");DBG.println(pid_ki,6);
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
  DBG.print("Lat "); DBG.print(pnt.lat); DBG.print("  Lon "); DBG.println( pnt.lon );
  DBG.println(F("autopilote_on"));
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
  //float kp, ki; //, kd;
  static float old_pid_error;
  float pid_error;
  float res;

  //kp=1.0;
  //ki=2.0*kp;
  //kd=0.125*kp;

  pid_error = nado-est; 
  // приводим градус ошибки к +-180 !!! VERY IMPORTMANT !!!!
  while (pid_error < -180) pid_error+=360;
  while (pid_error > 180) pid_error-=360;
  
  res = pid_kp*pid_error + pid_ki*(old_pid_error-pid_error);
  old_pid_error = pid_error;
  if (res > 100) res = 100;
  if (res < -100) res = -100;
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

    float to_point = GetDistanceInM( lat, lon, dest_lat, dest_lon );
    if ((to_point < 2.0) || (to_point > 502.0)) {
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

