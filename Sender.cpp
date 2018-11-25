#include "Sender.h"
#pragma comment (lib, "ws2_32.lib")

void fill_NLbuffer()
{
	for (int i = 0;i<10;i++)
	{
		network_layer_buffer[i].data = "It is a nice day";
	}
	packet_count += 10;
}

void from_network_layer(packet p[],seq_nr n)
{
	p[n].data = network_layer_buffer[packet_front++].data;
	
	if(packet_front == MAX_NLSIZE)
		packet_front = 0;
	
   packet_count--;
}

void to_network_layer(packet p[],seq_nr n)
{
	if(packet_count !=MAX_NLSIZE)
   {
	   if(packet_rear == MAX_NLSIZE-1) 
			packet_rear = -1;            

      network_layer_buffer [++packet_rear] = *p;
      packet_count++;
   }
}

void enable_network_layer()
{
	network_layer_enabled = 1;
}

void disable_network_layer()
{
	network_layer_enabled = 0;
}



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

	if (packet_count != 0 && network_layer_enabled == 1)
		insert_event(4);

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

void send_data(seq_nr frame_nr, seq_nr frame_expected, packet buffer[])
{		
	frame s;                                                                 
	s.info = buffer[frame_nr];//insert packet into frame
	s.seq = frame_nr;//insert sequence number into frame
	s.ack = (frame_expected + MAX_SEQ) % (MAX_SEQ + 1);//piggyback ack

	cout << s.info.data <<endl<< s.seq <<endl<< s.ack <<endl;
	to_physical_layer(&s); //transmit the frame
	//start_timer(frame_nr);  
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

static char * serialize_temp( char *buffer,  frame value)
{
  buffer = serialize_int(buffer, value.seq);
   buffer = serialize_int(buffer, value.ack);
  buffer = serialize_char(buffer, value.kind);
  buffer = serialize_packet(buffer, value.info);
  return buffer;
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