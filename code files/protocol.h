#include <stdio.h>

#define MAX_PKT 1024                                   /* determines packet size in bytes */
typedef struct {unsigned char data[MAX_PKT];} packet;  /* packet definition */

#define MAX_EVENT 50
typedef enum {frame_arrival, cksum_err, timeout, network_layer_ready,None} event_type;
static int event_buffer [MAX_EVENT];
static int event_front = 0;
static int event_rear = -1;
static int event_count = 0;

#define MAX_NLSIZE 1000  
static packet network_layer_buffer[MAX_NLSIZE]; 
static int packet_front = 0;
static int packet_rear = -1;
static int packet_count = 0;
static int network_layer_enabled;

/* Insert data to event_queue */
static void insert_event(int data);

/* Remove data from event_queue */
static int remove_event();

/* Wait for an event to happen; return its type in event. */
static void wait_for_event(event_type * event);

/* Allow the network layer to cause a network layer ready event. */
static void enable_network_layer(void);

/* Forbid the network layer from causing a network layer ready event. */
static void disable_network_layer(void);

/* Fetch a packet from the network layer for transmission on the channel. */
static void from_network_layer(packet * p);

/* Deliver information from an inbound frame to the network layer. */
static void to_network_layer(packet * p);
