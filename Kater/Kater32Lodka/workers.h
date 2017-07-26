// исполнительные устройства

void update_servo();
void init_servo();
void update_lights();

void setup_workers() {
  // servos
  gaz = 0;
  rul = 0;
  bunker = -127;
  update_servo();
  init_servo();
  
  // lights
  update_lights();
}

void update_servo() {
  uint16_t d;
  d = map( gaz, -127, 128, 700, 2200 );
  if ( Timer3.getCompare1() != d ) Timer3.setCompare1(d);
  d = map( rul, -127, 128, 700, 2200 );
  if ( Timer3.getCompare2() != d ) Timer3.setCompare2(d);
  d = map( bunker, -127, 128, 700, 2200 );
  if ( Timer3.getCompare3() != d ) Timer3.setCompare3(d);
}

void update_workers() {
  gaz = ctrl.gaz;
  rul = ctrl.rul;
  bunker = ctrl.light.bunker ? -127 : 128;
  update_servo();
  update_lights();
}

void init_servo() {
    pinMode(PA6, PWM); //ch1
    pinMode(PA7, PWM); //ch2
    pinMode(PB0, PWM); //ch3
    pinMode(PB1, PWM); //ch4
  
    Timer3.pause(); // while we configure
    Timer3.setPrescaleFactor(72);     // 1Mhz - 1mks
    Timer3.setChannel1Mode(TIMER_PWM);
    Timer3.setChannel2Mode(TIMER_PWM);
    Timer3.setChannel3Mode(TIMER_PWM);
    Timer3.setChannel4Mode(TIMER_PWM);
    Timer3.setOverflow(20000);   // Total time 20ms

    Timer3.setCount(0);         
    Timer3.resume();            
}

void update_lights(){
  
}

