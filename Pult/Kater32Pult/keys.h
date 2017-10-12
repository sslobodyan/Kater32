#define PIN_KEY PA6 // кнопка на джойстике

#define DELTA_JOY 100
#define CENTER_JOY 2047

uint32_t tm_button;
bool button, old_button, center_rul, center_gaz;

void setup_keys() {
  pinMode( PIN_KEY, INPUT_PULLUP);
  button = digitalRead( PIN_KEY );
  old_button = button;
}


void save_with_status(){
  uint16_t status = save_to_eprom();
  tft.setCursor(0, 0);
  tft.setTextSize( 1 );
  tft.setTextColor(ILI9341_RED,ILI9341_BLACK);
  tft.print(status, HEX);
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
            if ( is_left ) { 
              if (ctrl.sonar.delta) {
                ctrl.sonar.delta--; 
                shift_sonar_buffer(true);
              }
            } else { 
              if (ctrl.sonar.delta < 20 ) {
                ctrl.sonar.delta++; 
                shift_sonar_buffer(false);
              }
            }
            update_lineika();
            flash.sonar_delta = ctrl.sonar.delta;
            save_to_eprom( EEPROM_DELTA_SPEED );
            break;
            
          case mSonarTresh:
            if ( is_left ) { if (ctrl.sonar.treshold) ctrl.sonar.treshold--; }
            else { if (ctrl.sonar.treshold < 99 ) ctrl.sonar.treshold++; }
            update_treshold();
            flash.sonar_treshold = ctrl.sonar.treshold;
            save_to_eprom( EEPROM_TRESHOLD_TRIM );
            break;
            
          case mLight:
            if ( is_left ) ctrl.light.light--;
            else ctrl.light.light++;
            break;
            
          case mTrim:
            if ( is_left ) {
              if (flash.trim_rul > -50) {
                flash.trim_rul--;
                save_to_eprom( EEPROM_TRESHOLD_TRIM );
              }
            }
            else {
              if (flash.trim_rul < 50) {
                flash.trim_rul++;
                save_to_eprom( EEPROM_TRESHOLD_TRIM );
              }
            }
            break;

          case mTrap:
            if ( !is_left ) {
              open_bunker();
              is_menu = false;
            }
            break;
            
          case mClear:
            if ( ! is_left ) {
              flash.points[point_idx].lat = BAD_POINT;
              flash.points[point_idx].lon = BAD_POINT;
              save_to_eprom( EEPROM_POINT0 + point_idx );
              update_point();
              old_point_idx = 99;
            }
            break;
            
          case mGo:
            if ( !is_left ) {
              autopilot_on();
              is_menu = false;
            }
            break;
            
          case mSonarSpeed:
            ctrl.sonar.speed = !ctrl.sonar.speed;
            old.sonar.speed = 200;
            //update_lineika();
            flash.sonar_speed = ctrl.sonar.speed;
            save_to_eprom( EEPROM_DELTA_SPEED );
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
  if (is_menu) {
    if ( ( adc_rul > CENTER_JOY-DELTA_JOY ) && ( adc_rul < CENTER_JOY+DELTA_JOY ) ) {
      center_rul = true;
      if ( ( adc_gaz > CENTER_JOY-DELTA_JOY ) && ( adc_gaz < CENTER_JOY+DELTA_JOY ) ) {
        center_gaz = true;
      } else update_gaz();
    } else update_rul();    
  }

  button = digitalRead( PIN_KEY );
  if ( button ) { // не нажата или только что отпущена
    if (button_cnt >= BUTTON_CNT_PRESS) { // длинное нажатие - сброс бункера
       open_bunker();
       while (!digitalRead( PIN_KEY )) ;
    } else if (button_cnt >= BUTTON_CNT_CLICK) { // короткое нажатие - вход/выход меню
      if ( is_menu ) { // надо выходить
        is_menu = false;
        res = true;        
      } else {
        refresh_menu = true;
        is_menu = true;
      }
    }
    button_cnt = 0;
  } else {
    button_cnt++;
  }
  return res;
  
}

