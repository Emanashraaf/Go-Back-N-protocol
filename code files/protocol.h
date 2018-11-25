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

//typedef enum {false, true} boolean;                    /* boolean type */
typedef unsigned int seq_nr;                           /* sequence or ack numbers */
typedef struct {unsigned char data[MAX_PKT];} packet;  /* packet definition */
typedef enum {data, ack, nak} frame_kind;              /* frame_kind definition */
typedef struct {                                       /* frames are transported in this layer */
	frame_kind kind;                                   /* what kind of frame is it? */
	seq_nr seq;                                        /* sequence number */
	seq_nr ack;                                        /* acknowledgement number */
	packet info;                                       /* the network layer packet */
} frame;
/* Wait for an event to happen; return its type in event. */
void wait_for_event(event_type * event);
/* Fetch a packet from the network layer for transmission on the channel. */
void from_network_layer(packet * p);
/* Deliver information from an inbound frame to the network layer. */
void to_network_layer(packet * p);
/* Go get an inbound frame from the physical layer and copy it to r. */
void from_physical_layer(frame * r);
/* Pass the frame to the physical layer for transmission. */
void to_physical_layer(frame * s);
/* Start the clock running and enable the timeout event. */
void start_timer(seq_nr k);
/* Stop the clock and disable the timeout event. */
void stop_timer(seq_nr k);
/* Start an auxiliary timer and enable the ack timeout event. */
void start_ack_timer(void);
/* Stop the auxiliary timer and disable the ack timeout event. */
void stop_ack_timer(void);
/* Allow the network layer to cause a network layer ready event. */
void enable_network_layer(void);
/* Forbid the network layer from causing a network layer ready event. */
void disable_network_layer(void);
/* Macro inc is expanded in-line: increment k circularly. */
#define inc(k) if(k < MAX_SEQ) k = k + 1; else k = 0

//==========================================================================

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
