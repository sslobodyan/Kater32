void update_screen(bool refresh);
void update_main_screen(bool refresh);
void update_bort();
void update_tok();
void update_kurs();
void update_gps();
void update_point();
void refresh_static();
void update_treshold();
void update_lineika();
void update_sonar_data();
void drawHeading(uint8_t compas, uint16_t color);
void update_sat();
void update_sat_speed();

#define XUBORT 10
#define YUBORT 4

#define XIBORT 10
#define YIBORT 22

#define XSONAR 90
#define WSONAR 194
#define YSONAR 0
#define HSONAR 240

#define XDEEP 120
#define YDEEP 10

#define XPOINT 18
#define YPOINT 126

#define XSAT 18
#define YSAT 166

#define XSPEED 6
#define YSPEED 220

#define BUF_CNT WSONAR

#define TWIDTH 3 // ширина трешолда

#define XVECTOR  40
#define YVECTOR  80

uint8_t buf[BUF_CNT][60]; // карта эхолота
bool cnt_radio;

struct stVector {
  int8_t x;
  int8_t y;
};

const stVector vector[16] = {
  {0, -35},
  {13, -32},
  {25, -25},
  {32, -13},
  {35, 0},
  {32, 13},
  {25, 25},
  {13, 32},
  {0, 35},
  { -13, 32},
  { -25, 25},
  { -32, 13},
  { -35, 0},
  { -32, -13},
  { -25, -25},
  { -13, -32}
};

////////////////////////////////////////////////////////////////////

void screen_setup(){
  tft.begin(); 
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_YELLOW); 
  tft.setTextSize(2);
  tft.setRotation(3);
  //tft.println( sizeof(ctrl) ); tft.println( sizeof(tlm) ); delay(200);
#ifdef RRR  
  for (byte row=0; row < BUF_CNT; row++) {
    byte r = random(1,28);
    for (byte i=0; i<60; i++) {
      buf[row][i] = 0;
      if (row<60) {
        if (i<row) buf[row][i] = 0x00; else buf[row][i] = row; // debug
      } else {
        if (i<r) buf[row][i] = 0x00; else buf[row][i] = 0xFF; // debug
      }
    }
  }
#endif  
}

void update_screen(bool refresh=false){
  update_main_screen(refresh);
}

void update_main_screen(bool refresh=false) {  
  // мигающий квадратик
  if (cnt_radio) tft.fillRect(XVECTOR-36, YVECTOR-36, 8, 8, ILI9341_YELLOW);
  else tft.fillRect(XVECTOR-36, YVECTOR-36, 8, 8, ILI9341_BLACK);
  cnt_radio = !cnt_radio;
  
  if (refresh) refresh_static();
  if (old.bort != tlm.bort) update_bort();
  if (old.tok != tlm.tok) update_tok();
  if (old.kurs != tlm.kurs) update_kurs();
  if (old.sonar.cnt != tlm.sonar.cnt) update_sonar_data(); 
  if (old.sonar.delta != tlm.sonar.delta) update_lineika(); 
  if (old.sonar.speed != tlm.sonar.speed) update_lineika(); 
  if (old.sonar.treshold != tlm.sonar.treshold) update_treshold(); 

  update_point();  
  
  if (old.gps.sat.cnt != tlm.gps.sat.cnt) update_sat();  
  if (old.gps.sat.speed != tlm.gps.sat.speed) update_sat_speed();
}

void update_lineika(){
#define PIXEL_IN_M_0 49.2611 // 240 / (1450*120*0.00006/2)
#define PIXEL_IN_M_1 PIXEL_IN_M_0/2

  float pixel_in_m;
  uint16_t x;
  byte m=ctrl.sonar.delta, y;
  switch(tlm.sonar.speed) {
    case 0: pixel_in_m = PIXEL_IN_M_0; break;
    case 1: pixel_in_m = PIXEL_IN_M_1; break;
    default: pixel_in_m = PIXEL_IN_M_0;
  }
  x = XSONAR+WSONAR+1;
  float pixel=(float)YSONAR;
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK); 
  
  tft.fillRect(x, YSONAR, 320-x, 240-YSONAR, ILI9341_BLACK);

  while (pixel+pixel_in_m < HSONAR) {
    m += 1;
    pixel += pixel_in_m;
    y = YSONAR+pixel;
    tft.drawFastHLine(x, y, 10, ILI9341_WHITE);
    tft.setCursor(x+10, y-14);
    if (m<10) tft.print(" ");
    tft.print(m);
  }
  old.sonar.delta = tlm.sonar.delta;
  old.sonar.speed = tlm.sonar.speed;
}

void update_treshold() {
  for (byte i=0; i<TWIDTH; i++) {
    tft.drawFastVLine(XSONAR-TWIDTH-1+i, 240-tlm.sonar.treshold, tlm.sonar.treshold, ILI9341_WHITE);  
  }
  old.sonar.treshold = tlm.sonar.treshold;
}

void refresh_static(){
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK); 
  tft.setCursor( XUBORT+54, YUBORT );
  tft.print("v");
  tft.setCursor( XIBORT+54, YIBORT );
  tft.print("a");
  tft.drawFastVLine(XSONAR-1, YSONAR, HSONAR, ILI9341_WHITE);
  tft.drawFastVLine(XSONAR+WSONAR, YSONAR, HSONAR, ILI9341_WHITE);
  tft.drawFastVLine(XSONAR-TWIDTH-2, YSONAR, HSONAR, ILI9341_WHITE);  

  tft.drawFastHLine(0, YVECTOR-40,  XSONAR-TWIDTH-2, ILI9341_WHITE);  
  tft.drawFastHLine(0, YVECTOR+40,  XSONAR-TWIDTH-2, ILI9341_WHITE);  

  tft.setCursor(XSAT-14, YSAT);
  tft.print("@");
  tft.drawFastHLine(0, YSAT-4,  XSONAR-TWIDTH-2, ILI9341_WHITE);  
  tft.drawFastHLine(0, YSAT+18,  XSONAR-TWIDTH-2, ILI9341_WHITE);  

  tft.setCursor(XSPEED+40, YSPEED);
  tft.print("m/s");
  tft.drawFastHLine(0, YSPEED-4,  XSONAR-TWIDTH-2, ILI9341_WHITE);  
}

void update_bort(){
  float f = (float) tlm.bort / 10;
  tft.setCursor( XUBORT, YUBORT );
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK); 
  if (tlm.bort < 100) tft.print(" ");
  tft.print( f,1 );
  old.bort = tlm.bort;
}

void update_tok(){
  float f = (float) tlm.tok / 10;
  tft.setCursor( XIBORT, YIBORT );
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK); 
  if (tlm.tok < 100) tft.print(" ");
  tft.print( f,1 );
  old.tok = tlm.tok;
}

uint8_t kurs2compass( int16_t kurs ){
  uint8_t res;
  kurs += (180/16/2);
  if ( kurs < 0 ) kurs += 180;
  if ( kurs > 179 ) kurs -= 180;
  res = map(kurs,0,180,0,16);
  if (res>15) res = 0;
  return res;
}

void update_kurs(){
  uint8_t old_comp, comp;
  old_comp = kurs2compass( old.kurs );
  comp = kurs2compass( tlm.kurs );
  if ( old_comp != comp ) {
    drawHeading( old_comp , ILI9341_BLACK);
    drawHeading( comp , ILI9341_WHITE);
    old.kurs = tlm.kurs;    
  }
}

void update_gps(){
  
}

bool is_point_fill( stPoint point ) {
  bool res = true;
  if ( point.lat == 0 ) res = false; 
  if ( point.lon == 0 ) res = false;
  return res;
}

void update_point(){
  tft.setCursor(XPOINT, YPOINT);
  tft.setTextSize(4);
  if ( ! is_point_fill( points[point_idx] ) ) tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);
  else tft.setTextColor(ILI9341_BLACK,ILI9341_WHITE);
  tft.print( point_idx );
}

struct stLimit {
  byte tresh;
  byte r;
  byte g;
  byte b;
} lim[] = {
  {0, 0x00, 0x00, 0x00}, // черный
  {1, 0xFF, 0xFF, 0xFF}, // белый
  {5, 0x00, 0xFF, 0x00}, // зеленый
  {8, 0xFF, 0xFF, 0x00}, // желтый #FFFF00
  {13, 0xFF, 0x00, 0x00}  // красный
};

uint16_t get_color(uint8_t dat) {
  byte r=255, g=0, b=0;
  uint16_t clr;

  for (byte i=0; i<sizeof(lim)/4; i++) {
    if (dat < lim[i].tresh) {
      r = map(dat, lim[i-1].tresh, lim[i].tresh, lim[i-1].r, lim[i].r);
      g = map(dat, lim[i-1].tresh, lim[i].tresh, lim[i-1].g, lim[i].g);
      b = map(dat, lim[i-1].tresh, lim[i].tresh, lim[i-1].b, lim[i].b);
      break;
    }
  }

  clr = tft.color565(r, g, b);
  return clr;
}

void update_sonar_data(){
#define WONECHAR 17 // ширина одной цифры в большом шрифте
    
  uint16_t x;
  uint8_t dat, row, i, cnt=0, y;
  uint16_t col[240], x_deep;
  float f = (float) tlm.sonar.deep / 10.0 + tlm.sonar.delta;

  // сдвигаем влево буфер buf[WSONAR/2][30];
  int n = 60*(BUF_CNT-1);
  memcpy(&(buf[0][0]), &(buf[1][0]), n);
  
  //запоминаем текущее эхо
  memcpy(&(buf[BUF_CNT-1][0]), &(tlm.sonar.map[0]), 60);

  //for(byte q=0; q<60; q++) { DBG.print(tlm.sonar.map[q],HEX);DBG.print(","); }

  x = XSONAR;
  tft.setTextSize(3);
  tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);
  x_deep = XDEEP+WONECHAR;
  for (row=0; row < BUF_CNT; row++) {
    y = 0;
    for (i=0; i<60; i++) {
      bool pnt = false;
      dat = buf[row][i];
      byte b1, b2;
      b1 = dat >> 4;
      b2 = dat & 0b1111;
/*      
      if (row == BUF_CNT-1) { 
        DBG.print(dat,HEX);DBG.print(",");
        DBG.print(b1,HEX);DBG.print(",");DBG.print(b2,HEX);DBG.print(", "); 
      }
*/      
      uint16_t c = get_color(b1);
      col[y++] = c;
      col[y++] = c;

      c = get_color(b2);
      col[y++] = c;
      col[y++] = c;
    }
    //if (row == BUF_CNT-1) DBG.println();
    tft.setAddrWindow(x, YSONAR, x, YSONAR-1);    
    
    tft.drawDMABuffer(col, x, YSONAR, 1, 240-YSONAR); x += 1;

    // выводим глубину - каждую цифру сразу после ее заливки сонаром
    if ( x == x_deep ) {
      uint8_t d;
      switch (cnt) {
        case 0:
          d = f / 10;
          f -= d*10;
          if ( d ) {
            tft.setCursor(x-WONECHAR, YDEEP);
            tft.print(d);
          }
          cnt += 1;
          x_deep += WONECHAR;
          break;
        case 1:
          d = f;
          f -= d;
          tft.setCursor(x-WONECHAR-3, YDEEP);
          tft.print(d);
          cnt += 1;
          x_deep += WONECHAR;
          break;
        case 2:          
          // теперь десятичная точка
          tft.print(".");
          cnt += 1;
          x_deep += WONECHAR; 
          break;
        default:
          d = f*10;
          tft.setCursor(x-WONECHAR, YDEEP);
          tft.print(d);
      }
    }

  }
  old.sonar.cnt = tlm.sonar.cnt;
}

void drawHeading(uint8_t compas, uint16_t color) {
  // рисуем направление кораблика, вверх - строго от себя
  uint8_t p1, p2, p3, i;
  p1 = compas;
  p2 = compas + 7;
  if (p2 > 15) p2 -= 16;
  p3 = compas + 9;
  if (p3 > 15) p3 -= 16;
  tft.fillTriangle( //fillTriangle drawTriangle
    XVECTOR + vector[p1].x,
    YVECTOR + vector[p1].y,
    XVECTOR + vector[p2].x,
    YVECTOR + vector[p2].y,
    XVECTOR + vector[p3].x,
    YVECTOR + vector[p3].y,
    color
  );
  // рисуем точечный круг
  for (i = 0; i < 16; i++) {
    tft.drawPixel(XVECTOR + vector[i].x, YVECTOR + vector[i].y  , color);
  }
}

void update_sat(){
  tft.setCursor(XSAT, YSAT);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);
  if (tlm.gps.sat.cnt<10) tft.print(" ");
  tft.print(tlm.gps.sat.cnt);
  if (tlm.gps.sat.fix) tft.print(" 3d");
  else  tft.print(" --");
  old.gps.sat.cnt = tlm.gps.sat.cnt;  
}

void update_sat_speed(){
  tft.setCursor(XSPEED, YSPEED);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);
  float f = (float) tlm.gps.sat.speed / 10;
  if (f<10) tft.print(f,1);
  else tft.print("---");
  old.gps.sat.speed = tlm.gps.sat.speed;    
}

