#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512
#pragma comment(lib,"WS2_32.lib");

struct addrinfo *result = NULL,
             *ptr = NULL,
             hints;

int main()
{
	WSAData wsadata;
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;
	char *sendbuf = "1,2,3,4!!";
	char recvbuf [512];

	iResult=WSAStartup(MAKEWORD(2,2),&wsadata);
	if(iResult!=0)
	{
		std::cout<<"WSAStartup failed"<<iResult<<std::endl;
		getchar();
		return 1;
	}

	ZeroMemory( &hints, sizeof(hints) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	char arr[20]="localhost";

	iResult = getaddrinfo(arr, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		std::cout<<"getaddrinfo failed: "<<iResult;
	    getchar();
		WSACleanup();
	    return 1;
	}

	SOCKET ConnectSocket = INVALID_SOCKET;
	ptr=result;
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
    ptr->ai_protocol);
	
	if (ConnectSocket == INVALID_SOCKET)
	{
		std::cout<<"Error at socket(): "<<WSAGetLastError();
		getchar();
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
	    closesocket(ConnectSocket);
	    ConnectSocket = INVALID_SOCKET;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) 
	{
		std::cout<<"Unable to connect to server!"<<std::endl;
		getchar();
		WSACleanup();
		return 1;
	}



	for(;;)
	{
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    std::cout<<recvbuf<<std::endl;
	}
	
	iResult = shutdown(ConnectSocket, SD_SEND);
	
	if (iResult == SOCKET_ERROR) 
	{
		std::cout<<"shutdown failed: "<<WSAGetLastError();
		getchar();
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	if (iResult == 0)
		std::cout<<"Connection closed\n";
    else
		std::cout<<"recv of client failed: "<<WSAGetLastError();
	
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) 
	{
		std::cout<<"shutdown failed: "<<WSAGetLastError();
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	
	closesocket(ConnectSocket);
	WSACleanup();
	getchar();
	return 0;
}