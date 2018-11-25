#include <iostream>
#include <string>
#include <time.h>
#include <WS2tcpip.h>
#include <conio.h>
using namespace std;

/* Network Layer */
#define MAX_PKT 1024                                   /* determines packet size in bytes */
typedef struct {string data;} packet;                  /* packet definition */
typedef unsigned int seq_nr;                           /* sequence or ack numbers */

#define MAX_NLSIZE 1000  
static packet network_layer_buffer[MAX_NLSIZE]; 
static int packet_front = 0;
static int packet_rear = -1;
static int packet_count = 0;
static int network_layer_enabled;

/* fill the buffer of the network layer */
void fill_NLbuffer();

/* Allow the network layer to cause a network layer ready event. */
 void enable_network_layer();

/* Forbid the network layer from causing a network layer ready event. */
void disable_network_layer();

/* Fetch a packet from the network layer for transmission on the channel. */
void from_network_layer(packet p[],seq_nr n);

/* Deliver information from an inbound frame to the network layer. */
void to_network_layer(packet p[],seq_nr n);


/* Events */
#define MAX_EVENT 50
typedef enum {frame_arrival, cksum_err, timeout, network_layer_ready,None} event_type;
static int event_buffer [MAX_EVENT];
static int event_front = 0;
static int event_rear = -1;
static int event_count = 0;

/* Insert data to event_queue */
void insert_event(int data);

/* Remove data from event_queue */
int remove_event();

/* Wait for an event to happen; return its type in event. */
void wait_for_event(event_type * event);


/* Frame Transmission */
#define MAX_SEQ 7
typedef enum {data, ack, nak} frame_kind;//frame_kind definition
typedef struct 
{                                       
	frame_kind kind;                                   
	seq_nr seq;                                       
	seq_nr ack;                                      
	packet info;                                       
} frame;

/* Construct and send a data frame */
void send_data(seq_nr frame_nr, seq_nr frame_expected, packet buffer[]);

/* Go get an inbound frame from the physical layer and copy it to r */
void from_physical_layer(frame * r);

/* Pass the frame to the physical layer for transmission */
void to_physical_layer(frame * s);

#define inc(k) if(k < MAX_SEQ) k = k + 1; else k = 0