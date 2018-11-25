#include <iostream>
#include <WS2tcpip.h>
#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <conio.h>
#define MAX_PKT 1024  

#pragma comment (lib, "ws2_32.lib")

using namespace std;

typedef unsigned int seq_nr;                         
    typedef struct {unsigned char data[MAX_PKT];} packet;  
    typedef enum {data, ack, nak} frame_kind;   
	
	typedef struct {                                       
	frame_kind kind;                                   
	seq_nr seq;                                       
	seq_nr ack;                                       
	packet info;    
} frame;

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
          {
			  buffer[i] = value.data[i];		
	      }	
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

void main()
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
	//==================================================
	
	char buf[4096];
	char buf2[4096];
	int x = 0;
	frame v ;
	packet p = {'1','0','1','1'};
	v.seq = 1024;
	v.ack = 1;
	v.kind = ack;
	v.info = p ;
	 char * po = serialize_temp(buf2, v);
	while (true)
	{
		ZeroMemory(buf2, 4096);
		send(clientSocket, buf2, 4096 , 0);					
	}
		
	// Close the socket
	closesocket(clientSocket);

	// Cleanup winsock
	WSACleanup();
	_getch();
	system("pause");
}

