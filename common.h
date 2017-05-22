#define BASESTATION 0
#define ACCEL_SENSOR 1
#define TEMP_SENSOR 2

#define SENSOR_ON 1
#define SENSOR_OFF 0

static struct broadcast_conn broadcast;

struct message {
  int8_t id;
  int8_t msg;
};

static struct etimer et;
int onFlag = 1;
