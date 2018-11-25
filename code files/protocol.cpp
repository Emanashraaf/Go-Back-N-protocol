/* Protocol 5 (Go-back-n) allows multiple outstanding frames. The sender may transmit up
to MAX_SEQ frames without waiting for an ack. In addition, unlike in the previous
protocols, the network layer is not assumed to have a new packet all the time. Instead,
the network layer causes a network_layer_ready event when there is a packet to send. */
#define MAX_SEQ 7
#define MAX_PKT 1024 
//typedef enum {frame_arrival, cksum_err, timeout, network_layer_ready} event_type;
#include "protocol.h"
#include <time.h>
#include <iostream>
#include <WS2tcpip.h>
#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <conio.h>
 

#pragma comment (lib, "ws2_32.lib")

using namespace std;

int times ,timeout_enabled = 0;
int start ,eend;

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
	times = clock();
	if(times > eend)
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
	eend = start + 10;
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

char* serialize_int( char* buffer, seq_nr value)
{
	buffer[0] = value >> 24;
    buffer[1] = value >> 16;
    buffer[2] = value >> 8;
    buffer[3] = value;

    return(buffer + 4) ;
}

char* serialize_packet( char* buffer, packet value)
{
	int i;
	for( i=0;i< sizeof(value.data) ;i++)
		buffer[i] = value.data[i];
    return(buffer + i) ;
}


char * serialize_char( char *buffer, frame_kind value)
{
	
	   buffer[0] = value;
       return buffer + 1;
}

 char * serialize_temp( char *buffer,  frame value)
{
  buffer = serialize_int(buffer, value.seq);
   buffer = serialize_int(buffer, value.ack);
  buffer = serialize_char(buffer, value.kind);
  buffer = serialize_packet(buffer, value.info);
  return buffer;
}

void from_physical_layer(frame * r)
{
	string ipAddress = "127.0.0.1";			// IP Address of the server
	int port = 54000;						// Listening port # on the server

	// Initialize WinSock
	WSAData dataa;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &dataa);
	if (wsResult != 0)
	{
		cerr << "Can't start Winsock, Err #" << wsResult << endl;
		_getch();
		return;
	}

	// Create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cerr << "Can't create socket, Err #" << WSAGetLastError() << endl;
		WSACleanup();
		_getch();
		return;
	}

	// Fill in a hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	// Connect to server
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		cerr << "Can't connect to server, Err #" << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		_getch();
		return;
	}

	char buf[4096];
	int a[32] , b[32] ;
	
		// Wait for response
		ZeroMemory(buf, 4096);
		int bytesReceived = recv(sock, buf, 4096, 0);
		if (bytesReceived > 0)
		{
			// Echo response to console
			int j = 0 ; string sum ;
	                    for(int l= 3 ; l >= 0; l--)    
                            {    
	                     	 if (buf[l]==0)
		                 	  for(int i=0; i< 8; i++)    
                                 { 
                                   a[j] = 0;
				                   j++;
		                         }

		                     else
			                  for(int k=0; k< 8; k++)    
                                 { 
                                    a[j]=buf[l]%2;    
			                        buf[l] = buf[l]/2;  
			                        j++;
		                         }
	                           }
	                     for(j=j-1 ;j >=0 ;j--)
							 sum+= to_string(a[j]) ; 
		                 r->seq = stoull(sum, 0, 2);
		               // cout<<" seq-no " << r->seq << endl;
//=========================================================================================
						
						int d = 0 ; string sum2 ;
	                    for(int l= 7 ; l >= 4; l--)    
                            {    
	                     	 if (buf[l]==0)
		                 	  for(int i=0; i< 8; i++)    
                                 { 
                                   b[d] = 0;
				                   d++;
		                         }

		                     else
			                  for(int k=0; k< 8; k++)    
                                 { 
                                    b[d]=buf[l]%2;    
			                        buf[l] = buf[l]/2;  
			                        d++;
		                         }
	                           } 
	                     for(d=d-1 ;d >=0 ;d--)
							 sum2+= to_string(b[d]) ; 
		                r->ack = stoull(sum2, 0, 2);
		               // cout<<" ack " << r->ack << endl;
						
//=====================================================================================
						if (buf[8]==0)
						 {
							r->kind = data ;
						   // cout<<" frame-kind is data" << endl;
						 }
						 							
						else if(buf[8]==1)
						 {
							r->kind = ack ;
						  // cout<<" frame-kind is ack" << endl;
						 }
						   
						else
						  {
							r->kind = nak ;
						  // cout<<" frame-kind is nak" << endl;
						  }
//====================================================================================
                        for(int m=0;m< 1023;m++)
	                     {
							 r->info.data[m] =  buf[m+9];
							// cout<< r-> info.data[m];
						 }
				}

	// Gracefully close down everything
	closesocket(sock);
	WSACleanup();	
}

void to_physical_layer(frame * s)
{
	// Initialze winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		cerr << "Can't Initialize winsock! Quitting" << endl;
		_getch();
		return;
	}
	
	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting" << endl;
		_getch();
		return;
	}

	// Bind the ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton .... 
	
	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell Winsock the socket is for listening 
	listen(listening, SOMAXCONN);

	// Wait for a connection
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

	char host[NI_MAXHOST];		// Client's remote name
	char service[NI_MAXSERV];	// Service (i.e. port) the client is connect on

	ZeroMemory(host, NI_MAXHOST); // same as memset(host, 0, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host << " connected on port " << service << endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " <<
			ntohs(client.sin_port) << endl;
	}

	// Close listening socket
	closesocket(listening);
	//================================================================================
	
	char buf[4096];
	char * po = serialize_temp(buf, *s);
	send(clientSocket, buf, 4096 , 0);					
		
	// Close the socket
	closesocket(clientSocket);

	// Cleanup winsock
	WSACleanup();
	
}


static bool between(seq_nr a, seq_nr b, seq_nr c)
{
	/* Return true if a <= b < c circularly; false otherwise. */
	if(((a <= b) && (b < c)) || ((c < a) && (a <= b)) || ((b < c) && (c < a)))
		return(true);
	else
		return(false);
}
static void send_data(seq_nr frame_nr, seq_nr frame_expected, packet buffer[])
{
	/* Construct and send a data frame. */
	frame s;                                                                           /* scratch variable */
	s.info = buffer[frame_nr];                                                         /* insert packet into frame */
	s.seq = frame_nr;                                                                  /* insert sequence number into frame */
	s.ack = (frame_expected + MAX_SEQ) % (MAX_SEQ + 1);                                /* piggyback ack */
	to_physical_layer(&s);                                                             /* transmit the frame */
	start_timer(frame_nr);                                                             /* start the timer running */
}
void protocol5(void)
{
	seq_nr next_frame_to_send;                                                         /* MAX_SEQ > 1; used for outbound stream */
	seq_nr ack_expected;                                                               /* oldest frame as yet unacknowledged */
	seq_nr frame_expected;                                                             /* next frame_expected on inbound stream */
	frame r;                                                                           /* scratch variable */
	packet buffer[MAX_SEQ + 1];                                                        /* buffers for the outbound stream */
	seq_nr nbuffered;                                                                  /* number of output buffers currently in use */
	seq_nr i;                                                                          /* used to index into the buffer array */
	event_type event;
	enable_network_layer();                                                            /* allow network_layer_ready events */
	ack_expected = 0;                                                                  /* next ack_expected inbound */
	next_frame_to_send = 0;                                                            /* next frame going out */
	frame_expected = 0;                                                                /* number of frame_expected inbound */
	nbuffered = 0;                                                                     /* initially no packets are buffered */
	while(true) {
		wait_for_event(&event);                                                        /* four possibilities: see event_type above */

		switch(event) {
			case network_layer_ready:                                                  /* the network layer has a packet to send */
			/* Accept, save, and transmit a new frame. */
				from_network_layer(&buffer[next_frame_to_send]);                       /* fetch new packet */
				nbuffered = nbuffered + 1;                                             /* expand the sender’s window */
				send_data(next_frame_to_send, frame_expected, buffer);                 /* transmit the frame */
				inc(next_frame_to_send);                                               /* advance sender’s upper window edge */
				break;
			case frame_arrival:                                                        /* a data or control frame has arrived */
				from_physical_layer(&r);                                               /* get incoming frame from_physical_layer */
				if(r.seq == frame_expected) {
				/* Frames are accepted only in order. */
					to_network_layer(&r.info);                                         /* pass packet to_network_layer */
					inc(frame_expected);                                               /* advance lower edge of receiver’s window */
				}
				/* Ack n implies n − 1, n − 2, etc. Check for this. */
				while(between(ack_expected, r.ack, next_frame_to_send)) {
				/* Handle piggybacked ack. */
					nbuffered = nbuffered - 1;                                         /* one frame fewer buffered */
					stop_timer(ack_expected);                                          /* frame_arrived intact; stop_timer */
					inc(ack_expected);                                                 /* contract sender’s window */
				}
				break;
			case cksum_err: break;                                                     /* just ignore bad frames */
			case timeout:                                                              /* trouble; retransmit all outstanding frames */
				next_frame_to_send = ack_expected;                                     /* start retransmitting here */
				for (i = 1; i <= nbuffered; i++) {
					send_data(next_frame_to_send, frame_expected, buffer);             /* resend frame */
					inc(next_frame_to_send);                                           /* prepare to send the next one */
				}
		}
		if(nbuffered < MAX_SEQ)
			enable_network_layer();
		else
			disable_network_layer();
	}
}