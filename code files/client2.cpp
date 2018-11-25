#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include <conio.h>
#define MAX_PKT 1024  

typedef unsigned int seq_nr;                         
    typedef struct {unsigned char data[MAX_PKT];} packet;  
    typedef enum {data, ack, nak} frame_kind;   
	
	typedef struct {                                       
	frame_kind kind;                                   
	seq_nr seq;                                       
	seq_nr ack;                                       
	packet info;    
} frame;


#pragma comment(lib, "ws2_32.lib")

using namespace std;

void main()
{
	string ipAddress = "127.0.0.1";			// IP Address of the server
	int port = 54000;						// Listening port # on the server

	// Initialize WinSock
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
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

	// Do-while loop to send and receive data
	char buf[4096];
	int a[32] , b[32] ;
	frame r ;
	do
	{
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
		                 r.seq = stoull(sum, 0, 2);
		                cout<<" seq-no " << r.seq << endl;


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
		                r.ack = stoull(sum2, 0, 2);
		                cout<<" ack " << r.ack << endl;
						
//=====================================================================================
						if (buf[8]==0)
						 {
							r.kind = data ;
						    cout<<" frame-kind is data" << endl;
						 }
						 
							
						else if(buf[8]==1)
						 {
							r.kind = ack ;
						   cout<<" frame-kind is ack" << endl;
						 }
						   
						else
						  {
							r.kind = nak ;
						   cout<<" frame-kind is nak" << endl;
						  }
//====================================================================================
                        for(int m=0;m< 1023;m++)
	                     {
							 r.info.data[m] =  buf[m+9];
							 cout<< r.info.data[m];
						 }
				}

	} while (userInput.size() > 0);

	// Gracefully close down everything
	closesocket(sock);
	WSACleanup();
	_getch();
}
