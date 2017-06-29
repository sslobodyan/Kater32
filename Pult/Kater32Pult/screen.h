void update_screen(bool refresh);
void update_main_screen(bool refresh);
void update_ubort();
void update_ibort();
void update_kurs();
void update_sat();
void update_gps();
void update_point();
void refresh_static();
void update_treshold();
void update_lineika();
void update_sonar_deep();
void update_sonar_data();
void drawHeading(uint8_t compas, uint16_t color);

#define XUBORT 10
#define YUBORT 0

#define XIBORT 10
#define YIBORT 20

#define XSONAR 90
#define WSONAR 194
#define YSONAR 0
#define HSONAR 240

#define XDEEP 120
#define YDEEP 10

#define XPOINT 18
#define YPOINT 126

#define BUF_CNT WSONAR

#define TWIDTH 3 // ширина трешолда

#define XVECTOR  40
#define YVECTOR  78

uint8_t buf[BUF_CNT][30]; // карта эхолота

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
  tft.println( sizeof(radio) ); delay(200);
  for (byte row=0; row < BUF_CNT; row++) {
    byte r = random(1,28);
    for (byte i=0; i<30; i++) {
      buf[row][i] = 0;
      if (row<30) {
        if (i<row) buf[row][i] = 0x00; else buf[row][i] = row; // debug
      } else {
        if (i<r) buf[row][i] = 0x00; else buf[row][i] = 0xFF; // debug
      }
    }
  }
}

void update_screen(bool refresh=false){
  update_main_screen(refresh);
}

void update_main_screen(bool refresh=false) {
  if (refresh) refresh_static();
  if (old.ubort != radio.ubort) update_ubort();
  if (old.ibort != radio.ibort) update_ibort();
  if (old.gps.kurs != radio.gps.kurs) update_kurs();
  if (old.sonar_cnt != radio.sonar_cnt) update_sonar_data();  
  if (old.sonar_treshold != radio.sonar_treshold) update_treshold();  
  if ((old.sonar_speed != radio.sonar_speed) || (old.sonar_delta != radio.sonar_delta)) update_lineika();  
  if (old.gps.idx != radio.gps.idx) update_point();  
}

void update_lineika(){
#define PIXEL_IN_M_0 19.4
#define PIXEL_IN_M_1 38
#define PIXEL_IN_M_2 72

  float pixel_in_m;
  uint16_t x;
  byte m=radio.sonar_delta, y;
  switch(radio.sonar_speed) {
    case 0: pixel_in_m = PIXEL_IN_M_0; break;
    case 1: pixel_in_m = PIXEL_IN_M_1; break;
    default: pixel_in_m = PIXEL_IN_M_2;
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
  old.sonar_speed = radio.sonar_speed;
  old.sonar_delta = radio.sonar_delta;
}

void update_treshold() {
  for (byte i=0; i<TWIDTH; i++) {
    tft.drawFastVLine(XSONAR-TWIDTH-1+i, 240-radio.sonar_treshold, radio.sonar_treshold, ILI9341_RED);  
  }
  old.sonar_treshold = radio.sonar_treshold;    
}

void refresh_static(){
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK); 
  tft.setCursor( XUBORT+4*12, YUBORT );
  tft.print("v");
  tft.setCursor( XIBORT+4*12, YIBORT );
  tft.print("a");
  tft.drawFastVLine(XSONAR-1, YSONAR, HSONAR, ILI9341_WHITE);
  tft.drawFastVLine(XSONAR+WSONAR, YSONAR, HSONAR, ILI9341_WHITE);
  tft.drawFastVLine(XSONAR-TWIDTH-2, YSONAR, HSONAR, ILI9341_WHITE);  
}

void update_ubort(){
  float f = (float) radio.ubort / 10;
  tft.setCursor( XUBORT, YUBORT );
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK); 
  if (radio.ubort < 100) tft.print(" ");
  tft.print( f,1 );
  old.ubort = radio.ubort;
}

void update_ibort(){
  float f = (float) radio.ibort / 10;
  tft.setCursor( XIBORT, YIBORT );
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK); 
  if (radio.ibort < 100) tft.print(" ");
  tft.print( f,1 );
  old.ibort = radio.ibort;
}

uint8_t kurs2compass( int16_t kurs ){
  uint8_t res;
  //tft.setTextSize(1);
  //tft.setCursor(0,180); tft.print(kurs);
  kurs += (180/16/2);
  if ( kurs < 0 ) kurs += 180;
  if ( kurs > 179 ) kurs -= 180;
  //tft.print(" ");tft.print(kurs);
  res = map(kurs,0,180,0,16);
  if (res>15) res = 0;
  //tft.print(" ");tft.print(res);tft.print("   ");
  return res;
}

void update_kurs(){
  uint8_t old_comp, comp;
  old_comp = kurs2compass( old.gps.kurs );
  comp = kurs2compass( radio.gps.kurs );
  if ( old_comp != comp ) {
    drawHeading( old_comp , ILI9341_BLACK);
    drawHeading( comp , ILI9341_WHITE);
    old.gps.kurs = radio.gps.kurs;    
  }
}

void update_sat(){
  
}

void update_gps(){
  
}

void update_point(){
  tft.setCursor(XPOINT, YPOINT);
  tft.setTextSize(4);
  if (radio.gps.fill) tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);
  else tft.setTextColor(ILI9341_BLACK,ILI9341_WHITE);
  tft.print(radio.gps.idx);
  old.gps.idx = radio.gps.idx;  
}

void update_sonar_data(){
  uint16_t x, y;
  uint8_t dat, row, i;
  uint16_t col[240];
  x = XSONAR;
  for (row=0; row < BUF_CNT; row++) {
    y = 0;
    for (i=0; i<30; i++) {
      bool pnt = false;
      dat = buf[row][i];
      for (byte n=0; n<8; n++) {
        pnt = dat > 127 ? true : false;
        dat = dat << 1;
        if (pnt) {
          col[i*8+n] = ILI9341_ORANGE;
          //tft.drawPixel(x, y+YSONAR, ILI9341_RED);
          //tft.drawPixel(x+1, y+YSONAR, ILI9341_RED);      
        } else {
          col[i*8+n] = ILI9341_BLACK;
          //tft.drawPixel(x, y+YSONAR, ILI9341_BLACK);
          //tft.drawPixel(x+1, y+YSONAR, ILI9341_BLACK);                
        }
        y += 1;
      }
    }
    tft.setAddrWindow(x, YSONAR, x, YSONAR-1);    
    
    tft.drawDMABuffer(col, x, YSONAR, 1, 240-YSONAR); x += 1;

  }
  // сдвигаем влево буфер buf[WSONAR/2][30];
  int n = 30*(BUF_CNT-1);
  memcpy(&(buf[0][0]), &(buf[1][0]), n);

  update_sonar_deep();
  old.sonar_cnt = radio.sonar_cnt;
}

void update_sonar_deep(){
  tft.setCursor(XDEEP, YDEEP);
  tft.setTextSize(3);
  tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);
  float f = (float) radio.sonar_deep / 10.0 + radio.sonar_delta;
  tft.print(f,1);
  old.sonar_deep = radio.sonar_deep;
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

