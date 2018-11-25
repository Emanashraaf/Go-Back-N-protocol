#include "Sender.h"

int main ()
{
	event_type event;
	packet buffer[MAX_SEQ + 1];//buffers for the outbound stream
	seq_nr nbuffered = 0;//number of output buffers currently in use
	seq_nr next_frame_to_send = 0;//next frame going out
	seq_nr frame_expected = 0;//next frame_expected on inbound stream
	
	fill_NLbuffer();
	enable_network_layer();	               
	
	/*while(true) 
	{*/
		wait_for_event(&event);
		switch(event) 
		{
			/* the network layer has a packet to send */
			case network_layer_ready:
				from_network_layer(buffer,next_frame_to_send);//fetch new packet
				nbuffered = nbuffered + 1;//expand the sender’s window
				send_data(next_frame_to_send, frame_expected, buffer);//transmit the frame
				inc(next_frame_to_send);//advance sender’s upper window edge
				break;         

			default:
				break;
		}
	//}

}