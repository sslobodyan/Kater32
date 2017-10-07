#define XMENU XSONAR+1
#define YMENU 30
#define WIDTHMENU WSONAR-2
#define HEIGHTMENU 184
#define HEIGHTLINEMENU 26

enum e_menu1 {
  mPoint, mLight, mSonarWind, mSonarTresh, mTrap, mSave, mGo, mClear
};

struct stMenu {
  e_menu1 id;
  char title[20];
};

#define CNT_MENU 7
stMenu menu[CNT_MENU] = {
  {mPoint, "Точка"},
  {mGo, "Идти"},
  {mClear, "Очистить"},
  {mLight, "Свет"},
  {mSonarWind, "Окно"},
  {mSonarTresh, "Порог"},
  {mTrap, "Сброс"},
};

void show_menu1() {
  tft.setTextSize(3);
  for(byte i=0; i<CNT_MENU; i++) {
    if ( i == id_menu ) tft.setTextColor(ILI9341_WHITE, ILI9341_BLUE); 
    else tft.setTextColor(ILI9341_YELLOW); 
    tft.setCursor( XMENU+4, YMENU+2+HEIGHTLINEMENU*i );
    tft.print( utf8rus( menu[ i ].title ) );  
    switch ( menu[ i ].id ) {
      case mPoint: 
          tft.print(" "); tft.print( point_idx ); tft.print(" ");
          break;
      case mLight: 
          tft.print(" "); tft.print( ctrl.light.light );
          break;
      case mSonarWind:
          if ( ctrl.sonar.delta ) {
            tft.print(" "); tft.print( ctrl.sonar.delta ); tft.print(" ");
          } else tft.print("  ");
          break;
      case mSonarTresh: 
          tft.print(" "); tft.print( ctrl.sonar.treshold ); tft.print(" ");
          break;
    }
  }
}

void update_menu_screen() {
  if ( !refresh_menu ) {
    show_menu1();
    return;
  }
  refresh_menu = false;
  
  //tft.fillRect(XMENU, YMENU, WIDTHMENU, HEIGHTMENU, ILI9341_BLUE);
  for(uint16_t i=XMENU; i<WIDTHMENU+XMENU; i++) {
    tft.drawFastVLine(i, YMENU, HEIGHTMENU, ILI9341_BLUE);
    delayMicroseconds(200);
  }
  tft.drawRect(XMENU-1, YMENU-1, WIDTHMENU+2, HEIGHTMENU+2, ILI9341_WHITE);
  show_menu1();
  //tft.setCursor( XMENU+110, YMENU+2 ); tft.print(adc_gaz); tft.print(" "); tft.print(adc_rul); tft.print(" ");
}
