#include <stdio.h>
#include <string.h>
#include "contiki.h"
#include "net/rime/rime.h"
#include "dev/leds.h"

struct message {
	int8_t temp;
};

/* Declare our "main" process, the basestation_process */
PROCESS(basestation_process, "node basestation");
/* The basestation process should be started automatically when
 * the node has booted. */
AUTOSTART_PROCESSES(&basestation_process);

int busy = 0;

static void recv(struct broadcast_conn *c, const linkaddr_t *from) {
	struct message *m;
	m = packetbuf_dataptr();

	//printf("Received broadcast from %d.%d with value %d\n", from->u8[0], from->u8[1], m->temp);
	//printf("Temperature is above threshold (%d degrees)\n", m->temp);
	printf("Accelerometer has detected movement          \n");

	/*if(from->u8[0] == 140) {
		char* value = packetbuf_dataptr();
		printf("Received broadcast from %d.%d with value %s\n", from->u8[0], from->u8[1], value);
	}*/
}

/* Broadcast handle to receive and send (identified) broadcast
 * packets. */
static struct broadcast_conn bc;
/* A structure holding a pointer to our callback function. */
static struct broadcast_callbacks bc_callback = { recv };

/* Our main process. */
PROCESS_THREAD(basestation_process, ev, data) {
	PROCESS_BEGIN();
	broadcast_open(&bc, 130, &bc_callback);
	PROCESS_END();
}
