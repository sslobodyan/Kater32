#define PIN_I PA1
#define PIN_U PA0

#define RES_K_I 1.0
#define RES_K_U (4.7f+1.1f)/1.1f

#define MX_I 0.128906f // (20A / 2.5v/RES_K_I *3.3v /4096 *10)
#define MX_U 3.3f*RES_K_U/4096.0f*10.0f

int base_i;
int old_i, old_u;

void set_ADC2_injected(){
  int pinMapADCinU = PIN_MAP[PIN_U].adc_channel;
  int pinMapADCinI = PIN_MAP[PIN_I].adc_channel;

  adc_calibrate(ADC2);
  adc_set_sample_rate(ADC2, ADC_SMPR_239_5);
  
  ADC2->regs->CR2 = ADC_CR2_JEXTSEL;      //выбрать источником запуска разряд  JSWSTART 
  ADC2->regs->CR2 |=  ADC_CR2_JEXTTRIG;     //разр. внешний запуск инжектированной группы
  ADC2->regs->CR2 |=  ADC_CR2_CONT;         //режим непрерывного преобразования 
  
  ADC2->regs->CR1    |=  ADC_CR1_SCAN;         //режим сканирования (т.е. несколько каналов)
  ADC2->regs->CR1    |=  ADC_CR1_JAUTO;   //автомат. запуск инжектированной группы
  ADC2->regs->JSQR    =  (uint32_t)(2-1)<<20;  //задаем количество каналов 2 в инжектированной группе
  ADC2->regs->JSQR   |=  (uint32_t)0<<(5*pinMapADCinU);   //номер канала для первого преобразования A0 - U
  ADC2->regs->JSQR   |=  (uint32_t)1<<(5*pinMapADCinI);   //номер канала для второго преобразования A1 - I
  //ADC2->regs->JSQR   |=  (uint32_t)5<<(5*2);   //номер канала для третьего преобразования
  //ADC2->regs->JSQR   |=  (uint32_t)6<<(5*3);   //номер канала для четвертого преобразования
  ADC2->regs->CR2    |=  ADC_CR2_ADON;         //включить АЦП
  ADC2->regs->CR2    |=  ADC_CR2_JSWSTART;     //запустить процес преобразования
}

void init_sensors() {
#define SENSORS_SUM_CNT  50
  int tmp;
  uint32_t sum_i=0, sum_u=0;
  old_i = 0;
  old_u = 0;
  pinMode(PIN_I, INPUT_ANALOG);
  pinMode(PIN_U, INPUT_ANALOG);
  // настраиваем чтение по АЦП2 инжектированной группой
  set_ADC2_injected();
  delay(50);

  // измеряем значения каналов на холостом ходу
  //DBG.println("Calc base sensors");
  for (int i=0; i<SENSORS_SUM_CNT; i++) {
    
    tmp = ADC2->regs->JDR1;    //прочитать результат первого преобразования (в нашем случае канал 0)     /
    sum_u += tmp; 
    
    tmp = ADC2->regs->JDR2;    //прочитать результат второго преобразования (в нашем случае канал 1)
    sum_i += tmp; // на старте меряем напряжение токового датчика и берем его за 0  
    //DBG.println(tmp);
    //delay(20);
  }
  base_i = (uint32_t) sum_i / SENSORS_SUM_CNT;
  old_i = base_i;
  old_u = (uint32_t) sum_u / SENSORS_SUM_CNT;
  old_u = (float) MX_U * old_u;
  //DBG.print(F("base_i ")); DBG.println(base_i); DBG.print(F("U ")); DBG.print( ADC2->regs->JDR1 ); DBG.print("="); DBG.print(old_u); DBG.print(" MX_U="); DBG.println(MX_U);
}

void update_sensors() {
  int tmp;
  float u,i;
  tmp = ADC2->regs->JDR1;    //прочитать результат первого преобразования (в нашем случае канал 0)
  u = (float) MX_U * tmp + 0.5f;
  u = old_u*0.5 + u*0.5;
  old_u = u;
  //DBG.print("U=");DBG.println(old_u);

  tmp = ADC2->regs->JDR2;    //прочитать результат первого преобразования (в нашем случае канал 0)     /
  old_i = tmp*0.4 + old_i*0.6;
  i = (float) MX_I * abs( base_i - old_i );
  
  tlm.bort = u;
  tlm.tok = i;    

  //DBG.print("Tmp=");DBG.print(tmp);DBG.print(" Old_i=");DBG.print(old_i);DBG.print(" I=");DBG.print(i); DBG.print(" Tok=");DBG.println( tlm.tok );
}

