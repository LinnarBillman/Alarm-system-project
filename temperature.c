#include <stdio.h>
#include "contiki.h"
#include "dev/tmp102.h"
#include "net/rime/rime.h"

#define INTERVAL (CLOCK_SECOND / 0.5)
#define THRESHOLD 40

struct message {
	int16_t temp;
};

PROCESS(temp_process, "Temp process");
AUTOSTART_PROCESSES(&temp_process);

static void recv(struct broadcast_conn *c, const linkaddr_t *from) {
	struct message *m;
	m = packetbuf_dataptr();

	printf("Received broadcast from %d.%d with value %d\n", from->u8[0], from->u8[1], m->temp);
}

static struct broadcast_conn broadcast;
static const struct broadcast_callbacks broadcast_call = {recv};

PROCESS_THREAD(temp_process, ev, data) {
	PROCESS_EXITHANDLER(broadcast_close(&broadcast));
	PROCESS_BEGIN();

	broadcast_open(&broadcast, 130, &broadcast_call);
	
	static struct etimer et;
	struct message m;
	SENSORS_ACTIVATE(tmp102);
	
	while(1) {
		etimer_set(&et, INTERVAL);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		m.temp = tmp102.value(TMP102_READ);
		printf("temp is: %d\n", m.temp);
		
		if(m.temp >= THRESHOLD) {
			printf("Temp is higher than %d! Broadcasting...\n", THRESHOLD);
			packetbuf_copyfrom(&m, sizeof(struct message));
			broadcast_send(&broadcast);
		}
	}
	
	PROCESS_END();
}
