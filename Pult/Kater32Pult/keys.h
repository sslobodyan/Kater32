#define PIN_KEY PA6

#define DELTA_JOY 100
#define CENTER_JOY 2047

uint32_t tm_button;
bool button, old_button, center_rul, center_gaz;

void setup_keys() {
  pinMode( PIN_KEY, INPUT_PULLUP);
  button = digitalRead( PIN_KEY );
  old_button = button;
}

void update_rul() { // не в центре
    bool is_left = (adc_rul > CENTER_JOY+DELTA_JOY);
    if ( center_rul ) {
      center_rul = false;
      switch( menu[id_menu].id ) {
        case mPoint: 
            if ( is_left ) { if (point_idx) point_idx--; }
            else { if (point_idx < POINT_NUM-1 ) point_idx++; }
            break;
         case mSonarWind:
            if ( is_left ) { if (ctrl.sonar.delta) ctrl.sonar.delta--; }
            else { if (ctrl.sonar.delta < 20 ) ctrl.sonar.delta++; }
            update_lineika();
            break;
          case mSonarTresh:
            if ( is_left ) { if (ctrl.sonar.treshold) ctrl.sonar.treshold--; }
            else { if (ctrl.sonar.treshold < 200 ) ctrl.sonar.treshold++; }
            update_treshold();
            break;
          case mLight:
            if ( is_left ) ctrl.light.light--;
            else ctrl.light.light++;
            break;
          case mTrap:
            if ( ! is_left ) {
              points[point_idx].lat = tlm.gps.coord.lat;  
              points[point_idx].lon = tlm.gps.coord.lon;  
              old_point_idx = 99;
            }
            break;
          case mClear:
            if ( ! is_left ) {
              points[point_idx].lat = 0;
              points[point_idx].lon = 0;
              old_point_idx = 99;
            }
            break;
            
        default: ;
      }
    }  
}

void update_gaz() { // не в центре
    if ( center_gaz ) {
      center_gaz = false;
      if ( adc_gaz < CENTER_JOY-DELTA_JOY ) {
        if (id_menu) {
          id_menu--;
        } else id_menu=CNT_MENU-1;
      } else {
        if (id_menu < CNT_MENU-1) {
          id_menu++;
        } else id_menu=0;
      }
    }  
}

bool update_key() {
  bool res=false;
  if ( ( adc_rul > CENTER_JOY-DELTA_JOY ) && ( adc_rul < CENTER_JOY+DELTA_JOY ) ) {
    center_rul = true;
    if ( ( adc_gaz > CENTER_JOY-DELTA_JOY ) && ( adc_gaz < CENTER_JOY+DELTA_JOY ) ) {
      center_gaz = true;
    } else update_gaz();
  } else update_rul();
  
  button = digitalRead( PIN_KEY );
  if (old_button != button) {
    old_button = button;
    tm_button = millis() + 50;
  }
  if (tm_button  && ( tm_button < millis() ) ) { // кнопка в неизменном состоянии определенное время
    tm_button = 0;
    if (is_menu) {
      if ( !button ) {  // выйти из меню
        is_menu = false;
        res = true;
      }
    } else {
      if ( !button ) {  // зайти в меню
        refresh_menu = true;
        is_menu = true;
      }
    }
  }
  return res;
}

