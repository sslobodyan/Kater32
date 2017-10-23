// ssp-c китайский AT+BAUD8 115200 8N1


byte command_available=0;
uint btIdx = 0;
char btCommand[50]; // входной буфер команд
char wordCommand[50]; // сюда выбираем слово из команды


void do_bt_command();

void update_bt_command() {
  byte inp;
  
  while ( DBG.available() ) {
    if ( btIdx > sizeof(btCommand) - 1 ) { // переполнение буфера
      btIdx = 0;
      return;
    }
    inp = DBG.read();
    if (inp == 0x0D || inp == 0x0A ) {
      if ( DBG.available() ) {
        if ( DBG.peek() == 0x0A || DBG.peek() == 0x0D ) DBG.read();
      }
      inp = 0;

    }
    btCommand[ btIdx ] = inp;
    btIdx++;
    if (inp == 0) do_bt_command();
  }
}

byte get_word(byte idx) {
  byte cmd_idx = 0;
  while ( idx < command_available ) {
    if ( btCommand[idx] != ' ' ) {
      wordCommand[cmd_idx] = btCommand[idx];
      cmd_idx++;
      idx++;
    } else {
      wordCommand[cmd_idx] = 0;
      while ( btCommand[idx] == ' ' ) idx++;
      return idx;
    }
  }
  return 0;
}


void do_bt_command() { // разборка команды и выполнение
  btCommand[ btIdx + 1 ] = 0;
  command_available = btIdx;
  btIdx = 0;

  String stringOne, stringTwo;
  byte idx = get_word(0);
  stringOne = wordCommand; // первое слово
  if (idx) {
    idx = get_word(idx);
    stringTwo = wordCommand; // второе слово или параметр    
  }

  if (stringOne == F("showsonar")) {
    show_sonar = !show_sonar;
    DBG.print(show_sonar);
    DBG.println( F(" OK") );
  }
  else if (stringOne == F("showmax")) {
    show_max = !show_max;
    DBG.print(show_max);
    DBG.println( F(" OK") );
  }
  else if (stringOne == F("autocorr")) {
    auto_corr = !auto_corr;
    DBG.print(auto_corr);
    DBG.println( F(" OK") );
  }
  else if (stringOne == F("showrul")) {
    show_rul = !show_rul;
    DBG.print(show_rul);
    DBG.println( F(" OK") );
  }
  else if (stringOne == F("showpid")) {
    show_pid = !show_pid;
    DBG.print(show_pid);
    DBG.println( F(" OK") );
  }
  else if (stringOne == F("showdeep")) {
    show_deep = !show_deep;
    DBG.print(show_deep);
    DBG.println( F(" OK") );
  }
  else if (stringOne == F("show120")) {
    show_120 = !show_120;
    DBG.print(show_120);
    DBG.println( F(" OK") );
  }
  else if (stringOne == F("autohome")) {
    autohome = !autohome;
    DBG.print(autohome);
    DBG.println( F(" OK") );
  }
  else if (stringOne == F("at")) {
    DBG.println( F("OK") );
  }
  else if (stringOne == F("sonarkoeff")) {
    if (stringTwo.length() >= 1) {
      flash.sonar_koeff = stringTwo.toInt();
      save_to_eprom( EEPROM_SONAR_DNO );
    }
    DBG.println(flash.sonar_koeff);
    DBG.println( F(" OK") );
  }
  else if (stringOne == F("tresholddno")) {
    if (stringTwo.length() >= 1) {
      flash.treshold_dno = stringTwo.toInt();
      save_to_eprom( EEPROM_SONAR_DNO );
    }
    DBG.println(flash.treshold_dno);
    DBG.println( F(" OK") );
  }
  else if (stringOne == F("gaz")) {
    if (stringTwo.length() >= 1) {
      flash.gaz = stringTwo.toInt();
      save_to_eprom( EEPROM_GAZ );
    }
    DBG.println(flash.gaz);
    DBG.println( F(" OK") );
  }
  else if (stringOne == F("save_flash")) {
    save_to_eprom();
    DBG.println( F("OK") );
  }
  else if (stringOne == F("pidkp")) {
    if (stringTwo.length() >= 1) {
      flash.pid_kp = stringTwo.toInt();
      save_to_eprom( EEPROM_PIDP );
      init_pid();
    }
    DBG.print(flash.pid_kp);
    DBG.print("(");DBG.print(pid_kp,3);DBG.print(")");
    DBG.println( F(" OK") );
  }
  else if (stringOne == F("pidki")) {
    if (stringTwo.length() >= 1) {
      flash.pid_ki = stringTwo.toInt();
      save_to_eprom( EEPROM_PIDI );
      init_pid();
    }
    DBG.print(flash.pid_ki);
    DBG.print("(");DBG.print(pid_ki,3);DBG.print(")");
    DBG.println( F(" OK") );
  }
  else if (stringOne == F("pidkd")) {
    if (stringTwo.length() >= 1) {
      flash.pid_kd = stringTwo.toInt();
      save_to_eprom( EEPROM_PIDD );
      init_pid();
    }
    DBG.print(flash.pid_kd);
    DBG.print("(");DBG.print(pid_kd,3);DBG.print(")");
    DBG.println( F(" OK") );
  }
  else if (stringOne == F("minpi")) {
    if (stringTwo.length() >= 1) {
      flash.min_pi = stringTwo.toInt();
      save_to_eprom( EEPROM_MINPI );
      init_pid();
    }
    DBG.print(flash.min_pi);
    DBG.print("(");DBG.print(min_pi,3);DBG.print(")");
    DBG.println( F(" OK") );
  }
  else if (stringOne == F("maxpi")) {
    if (stringTwo.length() >= 1) {
      flash.max_pi = stringTwo.toInt();
      save_to_eprom( EEPROM_MAXPI );
      init_pid();
    }
    DBG.print(flash.max_pi);
    DBG.print("(");DBG.print(max_pi,3);DBG.print(")");
    DBG.println( F(" OK") );
  }
  else if (stringOne == F("help")) {
    DBG.println( F("pidkd") );
    DBG.println( F("pidki") );
    DBG.println( F("pidkp") );
    DBG.println( F("minpi") );
    DBG.println( F("maxpi") );
    DBG.println( F("tresholddno") );
    DBG.println( F("sonarkoeff") );
    DBG.println( F("showdeep") );
    DBG.println( F("showrul") );
    DBG.println( F("showsonar") );
    DBG.println( F("show120") );
    DBG.println( F("showmax") );
    DBG.println( F("showall") );
    DBG.println( F("autocorr") );
    DBG.println( F("autohome") );
    DBG.println( F("gaz") );
    DBG.println( F("OK") );
  }
  else if (stringOne == F("showall")) {
    DBG.print( F("pidkp ") );DBG.println(flash.pid_kp);
    DBG.print( F("pidki ") );DBG.println(flash.pid_ki);
    DBG.print( F("pidkd ") );DBG.println(flash.pid_kd);
    DBG.print( F("minpi ") );DBG.println(flash.min_pi);
    DBG.print( F("maxpi ") );DBG.println(flash.max_pi);
    DBG.print( F("tresholddno ") );DBG.println(flash.treshold_dno);
    DBG.print( F("sonarkoeff ") );DBG.println(flash.sonar_koeff);
    DBG.print( F("showdeep ") );DBG.println(show_deep);
    DBG.print( F("showrul ") );DBG.println(show_rul);
    DBG.print( F("showsonar ") );DBG.println(show_sonar);
    DBG.print( F("show120 ") );DBG.println(show_120);
    DBG.print( F("showmax ") );DBG.println(show_max);
    DBG.print( F("autocorr ") );DBG.println(auto_corr);
    DBG.print( F("autohome ") );DBG.println(autohome);
    DBG.print( F("gaz ") );DBG.println(flash.gaz);
    DBG.println( F("OK") );
  }
  
}

