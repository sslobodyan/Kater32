// исполнительные устройства

#define LED1 PB12
#define LED4 PB15

#define LED1_OFF pinMode(LED1, INPUT)
#define LED1_ON pinMode(LED1, OUTPUT)

#define LED4_OFF pinMode(LED4, INPUT)
#define LED4_ON pinMode(LED4, OUTPUT)

void update_servo();
void init_servo();
void update_lights();
void init_lights();

void setup_workers() {
  // servos
  DBG.println(F("setup_workers()"));
  gaz = 0;
  rul = 0;
  bunker = 0;
  update_servo();
  init_servo();
  init_lights();
  
  // lights
  update_lights();
}

void update_servo() {
  int16_t d;

  if ( update_autopilote() ) { // получаем коррекцию курса по автопилоту и дистанцию до цели
    tlm.gps.sat.autopilot = 1;
    rul = corr_autopilote; // в режиме автопилота рулем управляет робот
    gaz = 20;      
  } else {
    tlm.gps.sat.autopilot = 0;
  }
  tlm.rezerv = corr_autopilote; // TODO Debug

  d = map( gaz, -127, 128, 700, 2200 );
  if ( Timer3.getCompare1() != d ) Timer3.setCompare1(d);

  d = rul;
  d -= ctrl.trim;
  if (d > 127) d = 127;
  if (d < -126) d = -126;
  d = map( d, -127, 128, 700, 2200 );
  DBG.print("rul="); DBG.print(rul); DBG.print(" d="); DBG.println(d);
  if ( Timer3.getCompare2() != d ) Timer3.setCompare2(d);
  
  d = map( bunker, 0, 1, 700, 2200 );
  if ( Timer3.getCompare3() != d ) Timer3.setCompare3(d);
}

void update_workers() {
  gaz = ctrl.gaz;
  rul = ctrl.rul;
  bunker = ctrl.light.bunker; 
  
  if (ctrl.point.lat < BAD_POINT && ctrl.point.lon < BAD_POINT) { // если передаем точку, то включаем автопилот
    autopilote_on( ctrl.point );
  }

  update_servo();
  update_lights();

  //if (bunker) LED1_ON;
  
}

void init_servo() {
    
    DBG.println(F("init_servo()"));
    
    pinMode(PA6, PWM); //ch1
    pinMode(PA7, PWM); //ch2
    pinMode(PB0, PWM); //ch3
    pinMode(PB1, PWM); //ch4
  
    Timer3.pause(); // while we configure
    Timer3.setPrescaleFactor(71);     // 1Mhz - 1mks
    Timer3.setChannel1Mode(TIMER_PWM);
    Timer3.setChannel2Mode(TIMER_PWM);
    Timer3.setChannel3Mode(TIMER_PWM);
    Timer3.setChannel4Mode(TIMER_PWM);
    Timer3.setOverflow(19999);   // Total time 20ms

    Timer3.setCount(0);         
    Timer3.resume();            
}

void init_lights(){
  DBG.println(F("init_lights()"));
  digitalWrite( LED1, LOW );
  LED1_ON;
  digitalWrite( LED4, LOW );
  LED4_ON;
}

void update_lights(){
  ctrl.light.light & 0b01 ? (LED1_ON) : (LED1_OFF) ;
  ctrl.light.light & 0b10 ? (LED4_ON) : (LED4_OFF) ;
  //digitalWrite(LED1, ctrl.light.light & 0b01);
  //digitalWrite(LED4, ctrl.light.light & 0b10);
}

