#include <libmaple/dma.h>

#define SONAR_INT_PIN PA8
#define analogInPin PA4 // Analog input pin: any of LQFP44 pins (PORT_PIN), 10 (PA0), 11 (PA1), 12 (PA2), 13 (PA3), 14 (PA4), 15 (PA5), 16 (PA6), 17 (PA7), 18 (PB0), 19  (PB1)

#define BUF_LEN 1480
#define BUF120_LEN 120
uint16_t buf[BUF_LEN];
uint16_t buf120[BUF120_LEN];

#define TRESHOLD_MULTI 4 // усиление порога чувствительности - чем больше - тем меньше шумов


// End of DMA indication
volatile static bool sonar_newdata=false;

static void ExternSonarInt();
void takeSamples();

void setup_ADC()
{
  int pinMapADCin = PIN_MAP[analogInPin].adc_channel;
  adc_set_prescaler(ADC_PRE_PCLK2_DIV_8); // 9 МГц тактовая АЦП
  adc_set_sample_rate(ADC1, ADC_SMPR_239_5); // 239.5+12.5 = 252 такта на выборку 1/9000*252=0.028мс выборка
  adc_calibrate(ADC1);
  adc_set_reg_seqlen(ADC1, 1);
  ADC1->regs->SQR3 = pinMapADCin;
}

static void ExternSonarInt() { // пришло прерывание от триггера сонара
  detachInterrupt(SONAR_INT_PIN); // отключим сторожа
  takeSamples(); // запрашиваем новый сбор эха
}

static void DMA1_CH1_Event() {
  sonar_newdata = true;
  ADC1->regs->CR2 &= ~(ADC_CR2_CONT | ADC_CR2_DMA); // stop ADC
  dma_disable(DMA1, DMA_CH1); 
  tlm.sonar.cnt++;
}

void setup_DMA() {
  dma_init(DMA1);
  dma_attach_interrupt(DMA1, DMA_CH1, DMA1_CH1_Event);
  dma_setup_transfer( DMA1, DMA_CH1, 
                      &ADC1->regs->DR, DMA_SIZE_16BITS,
                      buf, DMA_SIZE_16BITS, 
                      (DMA_MINC_MODE | DMA_TRNS_CMPLT)
                     );// Receive buffer DMA
}

void takeSamples ()
{
  ADC1->regs->CR2 |= ADC_CR2_CONT | ADC_CR2_DMA; // Set continuous mode and DMA
  dma_set_num_transfers(DMA1, DMA_CH1, BUF_LEN);
  dma_enable(DMA1, DMA_CH1); // Enable the channel and start the transfer.
  ADC1->regs->CR2 |= ADC_CR2_SWSTART; // запускаем преобразование на постоянку
}

void sonar_setup(){
  LED1_ON;
  pinMode(analogInPin, INPUT_ANALOG);
  setup_ADC(); //Setup ADC peripherals 
  setup_DMA();  
  attachInterrupt(SONAR_INT_PIN, ExternSonarInt, RISING); // разрешим запрос эха
  DBG.println("sonar_setup()");
  LED1_OFF;
}

uint8_t press_dat(uint16_t dat) {
  if (dat > 3450) return 15;
  else if (dat > 1820) return 14;
  else if (dat > 964) return 13;
  else if (dat > 513) return 12;
  else if (dat > 274) return 11;
  else if (dat > 147) return 10;
  else if (dat > 79) return 9;
  else if (dat > 43) return 8;
  else if (dat > 24) return 7;
  else if (dat > 13) return 6;
  else if (dat > 8) return 5;
  else if (dat > 5) return 4;
  else if (dat > 3) return 3;
  else if (dat > 2) return 2;
  else if (dat > 1) return 1;
  else return 0;
}

uint8_t deep120() { // глубина по 120 буферу без учета дельты
  uint8_t width_dno = 4; // это 20 или 40 см
  int porog=ctrl.sonar.treshold + 5, level;
  uint8_t i,n,k;
  bool dno;
  float f;
  for (i=0; i<BUF120_LEN; i++) {
    k = (i+width_dno)<BUF120_LEN ? (i+width_dno) : BUF120_LEN;
    if ( buf120[i] > porog) {
      dno = true;
      level = buf120[i];
      for (n=i; n<k; n++) {
        if (level < buf120[n]) level = buf120[n];
        if ( buf120[n] < porog ) {// todo ???
          dno = false;
          break;
        }
      }
      if ( dno ) {
        f = (float) i * DM_IN_CNT_120 * (ctrl.sonar.speed + 1) + 0.5; // 120 точек 4.872м на скорости 0
        tlm.sonar.level = level >> 4; // уровень сигнала на дне приводим к байту
        //f = i;
        if (f>255) f=0;
          //DBG.print("Deep=");DBG.println(f);
        return f;
      }
    }
  }
  return 0;
}

void sonar_update_buf120() { // отобрать нужное окно из 120 замеров
  uint16_t start = (float) CNT_ON_1M_FROM_1480 * ctrl.sonar.delta + CNT_SKIP_FROM_SURFACE; // с какого отсчета начинается окно
  int idx=0, n, step, sr, tmp, mx, i;
  if (ctrl.sonar.speed == 0) step=2;
  else step=4;
  //DBG.print("120= ");
  for (i=start; idx<BUF120_LEN; i+=step) {
    /*
    sr = 0;
    for (n=0; n<step; n++) { // ищем среднюю для точки из 2 или 4 измерений
      sr += buf[i+n];
    }
    sr /= step;
    */
    if (i < CNT_CLEAR_FROM_SURFACE) sr = 0; else sr = buf[i];
    for (n=1; n<step; n++) { // ищем максимум для точки из 2 или 4 измерений
      if (i+n > CNT_CLEAR_FROM_SURFACE) {
        if (sr < buf[i+n]) sr = buf[i+n];  
      }
    }
    
    buf120[idx++] = sr;
    //DBG.print(sr);DBG.print(",");
  }   
  //DBG.println();
}

void clear2treshold120(){ // затереть все что ниже трешолда в 120 буфере
  uint16_t i, tr;
  tr = ctrl.sonar.treshold * TRESHOLD_MULTI;
  for (i=1; i<BUF120_LEN; i++) {
    if ( buf120[i] <= tr) buf120[i] = 0;
  }
}

void  sonar_pack_data(){
  uint16_t idx=0, i, tr;
/*  
  uint16_t mn;
  mn = 0xFFFF;
  for (i=1; i<sizeof(buf120); i++) {
    if ( mn > buf120[i] ) mn = buf120[i];
  }
  tlm.tok = mn;
*/
  // затираем все до уровня порога
  clear2treshold120();

  tlm.sonar.deep = deep120();

  uint8_t b1, b2, b3;
  //DBG.print("Sonar="); DBG.println(tlm.sonar.cnt);
  for (i=1; i<BUF120_LEN; i+=2) {
#ifdef PRESS_DAT    
    b1 = press_dat(buf120[i-1]);
    b2 = press_dat(buf120[i]);
#else    
    b1 = (int) buf120[i-1] >> 5;
    if (b1>15) b1=15;
    b2 = (int) buf120[i] >> 5;
    if (b2>15) b2=15;
#endif
    b3 = ((b1) << 4) + (b2);
    tlm.sonar.map[idx++] = b3;
    //DBG.print(b1);DBG.print(",");DBG.print(b2);DBG.print(",");
  }
  //DBG.println();
}

void print_buf(){
  int i, start=0, len=240;
  DBG.print("Buf(");DBG.print(start);DBG.print("-");DBG.print(len);DBG.print(") ");
  for(i=start; i<start+len; i++) {
    DBG.print(buf[i]);DBG.print(",");
  }
  DBG.println();
}

void sonar_update(){

  //print_buf();
  sonar_update_buf120();  
  sonar_pack_data(); // вычисляем глубину по 120 буферу и пакуем буфер в 60 байт.
}

uint8_t calc_deep( uint8_t idx ) { // не исп
   return (float) 1 + idx * 10 * M_IN_CNT_120 * (ctrl.sonar.speed+1); // + ctrl.sonar.delta; 
}



