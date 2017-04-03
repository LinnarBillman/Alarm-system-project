#include <stdio.h>
#include <string.h>
#include "contiki.h"
#include "net/rime.h"
#include "dev/leds.h"
#include "dev/cc2420.h"

#if 1
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif


/* Declare our "main" process, the basestation_process */
PROCESS(basestation_process, "node basestation");
/* The basestation process should be started automatically when
 * the node has booted. */
AUTOSTART_PROCESSES(&basestation_process);


int busy = 0;

static void recv(struct broadcast_conn *c, const rimeaddr_t *from) {
	while(busy);
	busy = 1;
	//char* recvString = (char*)packetbuf_dataptr();
	//int recvStringLen = strlen(recvString);
	PRINTF("%d %s\n",from->u8[0],(char*)packetbuf_dataptr());
	leds_toggle(LEDS_ALL);
	busy = 0;
}
/* Broadcast handle to receive and send (identified) broadcast
 * packets. */
static struct broadcast_conn bc;
/* A structure holding a pointer to our callback function. */
static struct broadcast_callbacks bc_callback = { recv };

/* Our main process. */
PROCESS_THREAD(basestation_process, ev, data) {
	PROCESS_BEGIN();
	/* Open the broadcast handle, use the rime channel
	 * defined by CLICKER_CHANNEL. */
	broadcast_open(&bc, 130, &bc_callback);
	/* Set the radio's channel to IEEE802_15_4_CHANNEL */
	cc2420_set_channel(16);
	/* Set the radio's transmission power. */
	cc2420_set_txpower(30);
	PROCESS_END();
}
