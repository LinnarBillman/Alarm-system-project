#include <stdio.h>
#include "contiki.h"
#include "dev/leds.h"
#include "dev/adxl345.h"
#include "net/rime/rime.h"
#include "common.h"
/*---------------------------------------------------------------------------*/
#define LED_INT_ONTIME        (CLOCK_SECOND / 2)
#define ACCM_READ_INTERVAL    CLOCK_SECOND
/*---------------------------------------------------------------------------*/

static process_event_t led_off_event;
static struct etimer led_etimer;
int j = 0;

int16_t temp = 0;

PROCESS(accel_process, "Test Accel process");
PROCESS(led_process, "LED handling process");
AUTOSTART_PROCESSES(&accel_process, &led_process);

static void recv(struct broadcast_conn *c, const linkaddr_t *from)
{
	struct message *m;
	m = packetbuf_dataptr();
	printf("Received broadcast from %d.%d with value %d\n", from->u8[0], from->u8[1], m->msg);
	if(m->id == BASESTATION){
	    if(m->msg == SENSOR_ON){
	      onFlag = 1;
	    }else if(m->msg == SENSOR_OFF){
	      onFlag = 0;
	    }
  	}
}
static const struct broadcast_callbacks broadcast_call = {recv};

void
print_int(uint16_t reg)
{
  if(reg & ADXL345_INT_FREEFALL) {
    printf("Freefall ");
  }
  if(reg & ADXL345_INT_INACTIVITY) {
    printf("InActivity ");
  }
  if(reg & ADXL345_INT_ACTIVITY) {
    printf("Activity ");
  }
  if(reg & ADXL345_INT_DOUBLETAP) {
    printf("DoubleTap ");
  }
  if(reg & ADXL345_INT_TAP) {
    printf("Tap ");
  }
  printf("\n");
}
/*---------------------------------------------------------------------------*/
/* accelerometer free fall detection callback */

void
accm_ff_cb(uint8_t reg)
{
  leds_on(LEDS_BLUE);
  process_post(&led_process, led_off_event, NULL);
  printf("~~[%u] Freefall detected! (0x%02X) -- ",
         ((uint16_t)clock_time()) / 128, reg);
  print_int(reg);
}
/*---------------------------------------------------------------------------*/
/* accelerometer tap and double tap detection callback */

void
accm_tap_cb(uint8_t reg)
{
  process_post(&led_process, led_off_event, NULL);
  if(reg & ADXL345_INT_DOUBLETAP) {
    leds_on(LEDS_GREEN);
    printf("~~[%u] DoubleTap detected! (0x%02X) -- ",
           ((uint16_t)clock_time()) / 128, reg);
  } else {
    leds_on(LEDS_RED);
    printf("~~[%u] Tap detected! (0x%02X) -- ",
           ((uint16_t)clock_time()) / 128, reg);
  }
  print_int(reg);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(led_process, ev, data) {
  PROCESS_BEGIN();
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == led_off_event);
    etimer_set(&led_etimer, LED_INT_ONTIME);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&led_etimer));
    leds_off(LEDS_RED + LEDS_GREEN + LEDS_BLUE);
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/* Main process, setups  */
PROCESS_THREAD(accel_process, ev, data)
{
  PROCESS_EXITHANDLER(broadcast_close(&broadcast));
  PROCESS_BEGIN();
  broadcast_open(&broadcast, 130, &broadcast_call);

  int16_t x, y, z;
  struct message m;	
  /* Register the event used for lighting up an LED when interrupt strikes. */
  led_off_event = process_alloc_event();

  /* Start and setup the accelerometer with default values, eg no interrupts
   * enabled.
   */
  SENSORS_ACTIVATE(adxl345);

  /* Register the callback functions for each interrupt */
  ACCM_REGISTER_INT1_CB(accm_ff_cb);
  ACCM_REGISTER_INT2_CB(accm_tap_cb);

  /* Set what strikes the corresponding interrupts. Several interrupts per
   * pin is possible. For the eight possible interrupts, see adxl345.h and
   * adxl345 datasheet.
   */
  accm_set_irq(ADXL345_INT_FREEFALL, ADXL345_INT_TAP + ADXL345_INT_DOUBLETAP);
     
  temp = adxl345.value(X_AXIS);
  temp = temp - 107;
  printf("temp: %d\n", temp);

  while(1) {
    if(onFlag){
      x = adxl345.value(X_AXIS);
      y = adxl345.value(Y_AXIS);
      z = adxl345.value(Z_AXIS);
      printf("x: %d y: %d z: %d\n", x, y, z);
    	
    	if(((temp+15) <x) || ((temp-15) > x)){
    		 m.msg = 2;
		 m.id = 1;
    		 packetbuf_copyfrom(&m, sizeof(struct message));
       		 broadcast_send(&broadcast);
		 printf("broadcasting\n");
    		 temp = x;
    	}
    }
  	
    etimer_set(&et, ACCM_READ_INTERVAL);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    
  }

  PROCESS_END();
}
