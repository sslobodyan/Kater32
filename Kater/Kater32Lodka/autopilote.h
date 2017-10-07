#define PIN_PID PA5

float src_lat, src_lon, dest_lat, dest_lon;

void set_destination(stPoint pnt) {
  dest_lat = (float) pnt.lat /  INT2FLOAT_MUX;
  dest_lon = (float) pnt.lon /  INT2FLOAT_MUX;
}

void set_source(stPoint pnt) {
  src_lat = (float) pnt.lat /  INT2FLOAT_MUX;
  src_lon = (float) pnt.lon /  INT2FLOAT_MUX;
}

void autopilote_on( stPoint pnt ) {
  set_destination( pnt );
}

void autopilote_off() {
  stPoint pnt;
  pnt.lat = BAD_POINT;
  pnt.lon = BAD_POINT;
  set_destination( pnt );
}

bool is_in_auto() {
  if ( (dest_lat != BAD_GRD) && (dest_lon != BAD_GRD ) ) return true;
  else return false;
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

void init_pid() {
  float a = analogRead(PIN_PID);
  pid_kp *= a/2048;
  pid_ki *= a/2048;
}

void update_autopilote() {
  if ( is_in_auto() ) {
    float lat = posllh.lat / INT2FLOAT_MUX;
    float lon = posllh.lon / INT2FLOAT_MUX;
    
    float heading = GetHeading( lat, lon, dest_lat, dest_lon );
    corr_autopilote = pid_corr(kurs_gps, heading);
  } else corr_autopilote = 0;
}

