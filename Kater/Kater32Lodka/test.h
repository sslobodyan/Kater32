static int8_t sonar=49;

void test_sonar() {
  uint8_t eho[] = {9,13,15,14,15,13,13,14,11,10,7,5,7,11,12,7,5,3,1,1,2,2,1,1};
  uint8_t buf[120];
  sonar += (random(5)-2);
  if (sonar >= 110) sonar = 109;
  if (sonar < 20) sonar = 20;

  tlm.sonar.deep = calc_deep( sonar );

  byte i=0;
  while (i<sonar) {
    buf[i] = 0;
    i++;
  }

  byte n=0;
  while ( n<sizeof(eho) ) {
    buf[i] = eho[n];
    i++;
    if (i==120) break;
    n++;
  }

  while ( i < 120 ) {
    buf[i] = 0;
    i++;
  }

  for (i=0; i<120; i+=2) {
    tlm.sonar.map[i/2] = (buf[i] << 4) + buf[i+1];
  }
  
}

void test_data() {
  //tlm.kurs += 1;
  //tlm.bort += 1;
  //tlm.gps.sat.speed += 1;
  //tlm.gps.sat.cnt = 15;
  //tlm.gps.sat.fix = 1;
  //tlm.gps.sat.second += 1;
  //ctrl.sonar.treshold = 2;
}



