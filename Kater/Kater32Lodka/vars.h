// переменные и константы

stControl ctrl; // управление с пульта
stTelemetry tlm; // телеметрия с катера

int8_t rul, gaz, bunker;

// эхолот
#define CNT_ON_1M_FROM_1480 49.26108f // 252/9000000*1450/2 = 0.0203m   1/0.0203=49.2610837
#define M_IN_CNT_120 0.0406 // 252/9000000*1450/2*2 = 0.0406m   // количество метров между двумя точками в буфере
#define DM_IN_CNT_120  M_IN_CNT_120*10.0f // 252/9000000*1450/2*2*10 = 0.406m   // количество дециметров между двумя точками в буфере
#define CNT_SKIP_FROM_SURFACE CNT_ON_1M_FROM_1480*2.55 // 3.45мс от триггера до начала сканирующего импульса (123)
#define CNT_CLEAR_FROM_SURFACE CNT_ON_1M_FROM_1480*0.7f + CNT_SKIP_FROM_SURFACE // отсекаем шум от поверхности

// правильные координаты
#define INT2FLOAT_MUX 10000000
#define BAD_GRD 200.0
#define BAD_POINT BAD_GRD * INT2FLOAT_MUX



uint32_t tm, tm_gps, tm_sensors, tm_no_radio=0, tm_auto_corr=0, tm_workers;

uint8_t base_heading = 0;
float kurs_gps = 0;
int8_t corr_autopilote=0; // коррекция автопилота для руля
#define CNT_HEADING_SAMPLES 10
uint16_t sum_base_heading = 0;
int8_t cnt_base_heading = CNT_HEADING_SAMPLES;

float pid_kp, pid_kd, pid_ki, min_pi, max_pi;
static float old_pid_error;
static float old_res_i;
float to_point; // расстояние от катера до целевой точки в режиме автопилота
float diam; // диаметр целевой точки в метрах

bool show_sonar = false, show_rul=false, show_deep=false, auto_corr=false, show_pid=false, show_max=false, show_120=false, autohome=true;
int cnt_no_radio=0;

int maximum; // max buf120

stPoint home_pnt;

void init_vars(){
  tlm.gps.coord.lat = BAD_POINT;
  tlm.gps.coord.lon = BAD_POINT;
  tlm.sonar.deep = 0;
  tlm.sonar.level = 0;
}


void init_pid();
