#define XMENU XSONAR+1
#define YMENU 1
#define WIDTHMENU WSONAR-2
#define HEIGHTMENU 238
#define HEIGHTLINEMENU 26

enum e_menu1 {
  mPoint, mLight, mSonarWind, mSonarTresh, mTrap, mSave, mGo, mClear, mPid, mSonarSpeed, mTrim
};

struct stMenu {
  e_menu1 id;
  char title[20];
};

#define CNT_MENU 8
stMenu menu_for_sonar[CNT_MENU] = {
  {mPoint, "Точка"},
  {mGo, "Идти"},
  {mClear, "Очистить"},
  {mLight, "Свет"},
  {mSonarWind, "Верх с"},
  {mSonarTresh, "Помехи"},
  {mSonarSpeed, "Высота"},
  {mTrap, "Сброс"},
};

void show_menu_sonar() {
  tft.setTextSize(3);
  for(byte i=0; i<CNT_MENU; i++) {
    if ( i == id_menu ) {
      tft.fillRect(XMENU, YMENU+HEIGHTLINEMENU*i, WIDTHMENU, HEIGHTLINEMENU, ILI9341_BLACK);
      tft.setTextColor(ILI9341_WHITE); 
    }
    else {
      tft.fillRect(XMENU, YMENU+HEIGHTLINEMENU*i, WIDTHMENU, HEIGHTLINEMENU, ILI9341_BLUE);
      tft.setTextColor(ILI9341_YELLOW); 
    }
    tft.setCursor( XMENU+4, YMENU+2+HEIGHTLINEMENU*i );
    tft.print( utf8rus( menu_for_sonar[ i ].title ) );  
    switch ( menu_for_sonar[ i ].id ) {
      case mPoint: 
          tft.print(" "); tft.print( point_idx ); tft.print(" ");
          break;
      case mLight: 
          tft.print(" "); tft.print( ctrl.light.light );
          break;
      case mSonarWind:
          tft.print(" "); tft.print( ctrl.sonar.delta ); tft.print(" ");
          break;
      case mSonarSpeed:
          if ( ctrl.sonar.speed ) {  
            tft.print( " 9m" ); //tft.print(" ");
          } else {
            tft.print( " 5m" ); //tft.print(" ");
          }
          break;
      case mSonarTresh: 
          tft.print(" "); tft.print( ctrl.sonar.treshold ); tft.print(" ");
          break;
    }
  }
}

#define CNT_MENU_KALIBR 5
stMenu menu_for_kalibr[CNT_MENU_KALIBR] = {
  {mPoint, "Точка"},
  {mGo, "Идти"},
  {mClear, "Очистить"},
  {mTrim, "Руль"},
  {mPid, "PID"}
};

void show_menu_kalibr() {
  tft.setTextSize(3);
  for(byte i=0; i<CNT_MENU_KALIBR; i++) {
    if ( i == id_menu ) {
      tft.fillRect(XMENU, YMENU+HEIGHTLINEMENU*i, WIDTHMENU, HEIGHTLINEMENU, ILI9341_BLACK);
      tft.setTextColor(ILI9341_WHITE); 
    }
    else {
      tft.fillRect(XMENU, YMENU+HEIGHTLINEMENU*i, WIDTHMENU, HEIGHTLINEMENU, ILI9341_BLUE);
      tft.setTextColor(ILI9341_YELLOW); 
    }
    tft.setCursor( XMENU+4, YMENU+2+HEIGHTLINEMENU*i );
    tft.print( utf8rus( menu_for_kalibr[ i ].title ) );  
    switch ( menu_for_kalibr[ i ].id ) {
      case mPoint: 
          tft.print(" "); tft.print( point_idx ); tft.print(" ");
          break;
      case mTrim:
          tft.print(" "); tft.print( ctrl.trim ); tft.print(" ");
          break;
      case mPid:
          tft.print(" "); tft.print( ctrl.pid ); tft.print(" ");
          break;
    }
  }
}

void update_menu_screen_sonar() {
  if ( first_refresh_menu ) {
    // первоначальная прорисовка
    first_refresh_menu = false;  
    for(uint16_t i=XMENU; i<WIDTHMENU+XMENU; i++) {
      tft.drawFastVLine(i, YMENU, HEIGHTMENU, ILI9341_BLUE);
      delayMicroseconds(200);
    }
    tft.drawRect(XMENU-1, YMENU-1, WIDTHMENU+2, HEIGHTMENU+2, ILI9341_WHITE);
  }
  if (refresh_menu) {
    show_menu_sonar();
    refresh_menu = false;
  }
}

void update_menu_screen_kalibr() {
  if ( first_refresh_menu ) {
    first_refresh_menu = false; 
    for(uint16_t i=XMENU; i<WIDTHMENU+XMENU; i++) {
      tft.drawFastVLine(i, YMENU, HEIGHTMENU, ILI9341_BLUE);
      delayMicroseconds(200);
    }
    tft.drawRect(XMENU-1, YMENU-1, WIDTHMENU+2, HEIGHTMENU+2, ILI9341_WHITE);
  }
  if (refresh_menu) {
    show_menu_kalibr();
    refresh_menu = false;
  }
}

