#include "protocol.h"
#include <time.h>
int time ,timeout_enabled = 0;
time_t start ,end;

void insert_event(int data) 
{
   if(event_count != MAX_EVENT)
   {
	   if(event_rear == MAX_EVENT-1) 
			event_rear = -1;            

      event_buffer [++event_rear] = data;
      event_count++;
   }
}

int remove_event() 
{
   int data = event_buffer[event_front++];
	
   if(event_front == MAX_EVENT)
      event_front = 0;
	
   event_count--;
   return data;  
}

void wait_for_event(event_type* event)
{
	time = clock();
	if(time > end)
		{
			timeout_enabled = 1;
		}

	if (packet_count != 0 && network_layer_enabled == 1)
		insert_event(4);

	while (event_count == 0)
	{
		// wait for an event
		*event = None;
	}

	switch (remove_event())
	{
		case 1:
			*event = frame_arrival;
			break;

		case 2:
			*event = cksum_err;
			break;

		case 3:
			*event = timeout;
			break;

		case 4:
			*event = network_layer_ready;
			break;
		
		default:
			*event = None;
			break;
	}
	 
}

void from_network_layer(packet * p)
{
	*p = network_layer_buffer[packet_front++];
	
	if(packet_front == MAX_NLSIZE)
		packet_front = 0;
	
   packet_count--;
}

void to_network_layer(packet * p)
{
	if(packet_count !=MAX_NLSIZE)
   {
	   if(packet_rear == MAX_NLSIZE-1) 
			packet_rear = -1;            

      network_layer_buffer [++packet_rear] = *p;
      packet_count++;
   }
}

void start_timer(seq_nr k)
{
	start = clock();
	end = start + 10;
}

void stop_timer(seq_nr k)
{
	timeout_enabled = 0;
}

void enable_network_layer()
{
	network_layer_enabled = 1;
}

void disable_network_layer()
{
	network_layer_enabled = 0;
}



