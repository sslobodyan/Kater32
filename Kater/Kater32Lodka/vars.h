// переменные и константы

stControl ctrl; // управление с пульта
stTelemetry tlm; // телеметрия с катера

#define GAZ_PIN PA6
#define RUL_PIN PA7
#define BUNKER_PIN PB0

int8_t rul, gaz, bunker;

#define CNT_ON_1M_FROM_1480 49.26108 // 252/9000000*1450/2 = 0.0203m   1/0.0203=49.2610837
#define M_IN_CNT_120 0.0406 // 252/9000000*1450/2*2 = 0.0406m   

