
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
  DBG.println(F("autopilote_on"));
}

void autopilote_off() {
  stPoint pnt;
  pnt.lat = BAD_POINT;
  pnt.lon = BAD_POINT;
  set_destination( pnt );
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
  res = pid_kp*pid_error + pid_ki*(old_pid_error-pid_error);
  old_pid_error = pid_error;
  if (res > 125) res = 125;
  if (res < -125) res = -125;
  return res;
}

bool update_autopilote() {
  if (rul > 30 || rul < -30) { // подергали рулем
    corr_autopilote = 0;
    autopilote_off();
    return false;    
  }
  if (gaz > 30 || gaz < -30) { // подергали газом
    corr_autopilote = 0;
    autopilote_off();
    return false;    
  }
  if ( is_in_auto() ) {
    float lat = posllh.lat / INT2FLOAT_MUX;
    float lon = posllh.lon / INT2FLOAT_MUX;
    
    float heading = GetHeading( lat, lon, dest_lat, dest_lon );
    corr_autopilote = pid_corr(kurs_gps, heading);

    float to_point = GetDistanceInM( lat, lon, dest_lat, dest_lon );
    if (to_point < 2.0) {
      autopilote_off();
      return false;
    } else return true;
  } else {
    corr_autopilote = 0;
    return false;
  }
}

