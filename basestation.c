#include <stdio.h>
#include <string.h>
#include "contiki.h"
#include "net/rime/rime.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"
#include "common.h"

PROCESS(basestation_process, "node basestation");
AUTOSTART_PROCESSES(&basestation_process);

int busy = 0;

static void recv(struct broadcast_conn *c, const linkaddr_t *from) {
	struct message *m;
	m = packetbuf_dataptr();

	if(m->id == ACCEL_SENSOR){
		printf("Accelerometer has detected movement          \n");
	} else if(m->id == TEMP_SENSOR){
		printf("Temp sensor has detected fire (%d degrees)   \n", m->msg);
	}
}

static struct broadcast_callbacks bc_callback = { recv };

PROCESS_THREAD(basestation_process, ev, data) {
	PROCESS_BEGIN();
	broadcast_open(&broadcast, 130, &bc_callback);

	SENSORS_ACTIVATE(button_sensor);

	struct message m;
	while(1){
		PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

		if(onFlag){
			m.id = BASESTATION;
			m.msg = SENSOR_OFF;
			packetbuf_copyfrom(&m, sizeof(struct message));
			broadcast_send(&broadcast);
			onFlag = 0;
		}else{
			m.id = BASESTATION;
			m.msg = SENSOR_ON;
			packetbuf_copyfrom(&m, sizeof(struct message));
			broadcast_send(&broadcast);
			onFlag = 1;
		}
	}
	
	PROCESS_END();
}
