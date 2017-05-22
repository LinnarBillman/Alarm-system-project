#include <stdio.h>
#include "contiki.h"
#include "dev/tmp102.h"
#include "net/rime/rime.h"
#include "common.h"

#define INTERVAL (CLOCK_SECOND / 0.5)
#define THRESHOLD 15

PROCESS(temp_process, "Temp process");
AUTOSTART_PROCESSES(&temp_process);

static void recv(struct broadcast_conn *c, const linkaddr_t *from) {
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

PROCESS_THREAD(temp_process, ev, data) {
	PROCESS_EXITHANDLER(broadcast_close(&broadcast));
	PROCESS_BEGIN();

	broadcast_open(&broadcast, 130, &broadcast_call);
	
	struct message m;
	int16_t temperature;

	SENSORS_ACTIVATE(tmp102);
	
	while(1) {
		etimer_set(&et, INTERVAL);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		if(onFlag){

			temperature = tmp102_read_temp_raw();
			m.msg = (temperature >> 8);
			printf("temp is: %d\n", m.msg);
			
			if(m.msg >= THRESHOLD) {
				m.id = 2;
				printf("Temp is higher than %d! Broadcasting...\n", THRESHOLD);
				packetbuf_copyfrom(&m, sizeof(struct message));
				broadcast_send(&broadcast);
			}
		}	
	}
	
	PROCESS_END();
}
