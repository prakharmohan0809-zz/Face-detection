#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

#pragma comment(lib, "Ws2_32.lib")

struct addrinfo *result = NULL, *ptr = NULL, hints;

int main() {

	WSADATA wsadata;
	int iresult;
	char* sendbuf="1,2,3,4!";
	int iSendResult;
	

	iresult=WSAStartup(MAKEWORD(2,2),&wsadata);
	
	if(iresult!=0)
	{
		std::cout<<"WSAStartup failed"<<std::endl;
		getchar();
		return 1;
	}
	
	ZeroMemory(&hints, sizeof (hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iresult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iresult != 0)
	{
		std::cout<<"getaddrinfo failed:"<<iresult<<std::endl;
		getchar();
		WSACleanup();
		return 1;
	}
	
	SOCKET ListenSocket=INVALID_SOCKET;
	ListenSocket=socket(result->ai_family,result->ai_socktype,result->ai_protocol);
	
	if(ListenSocket==INVALID_SOCKET)
	{
		std::cout<<"Error at socket():"<<WSAGetLastError()<<std::endl;
		freeaddrinfo(result);
		getchar();
		WSACleanup();
		return 1;
	}
	
	iresult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    
	if (iresult == SOCKET_ERROR)
	{
		std::cout<<"Bind failed with error:"<<WSAGetLastError()<<std::endl;
        getchar();
		freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
	
	freeaddrinfo(result);

	if (listen(ListenSocket,SOMAXCONN)==SOCKET_ERROR)
	{
		std::cout<<"Listen failed with error:"<<WSAGetLastError();
		getchar();
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	
	SOCKET ClientSocket;

	ClientSocket = accept(ListenSocket, NULL , NULL); //error??
	
	if (ClientSocket == INVALID_SOCKET)
	{
		std::cout<<"accept failed:"<<WSAGetLastError();
		getchar();
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	
	for(;;)
	{
	
		iresult = send(ClientSocket, sendbuf, (int) strlen(sendbuf), 0);		
	}
        iSendResult = send(ClientSocket, sendbuf, iresult, 0);
        if (iSendResult == SOCKET_ERROR)
		{
		std::cout<<"send failed from server: "<<WSAGetLastError();
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
        }
		std::cout<<"Bytes sent from server: "<<iSendResult;
		return 1;
		if(iresult==0)
		{
		std::cout<<"Connection closing.";
		return 1;
		}
		else 
		{
		std::cout<<"recv of server failed "<<WSAGetLastError();
        closesocket(ClientSocket);
        WSACleanup();
		return 1;
		}

	

	iresult = shutdown(ClientSocket, SD_SEND);
    if (iresult == SOCKET_ERROR) 
	{
	std::cout<<"shutdown failed with error: "<<WSAGetLastError();
    closesocket(ClientSocket);
    WSACleanup();
    return 1;
	}
	
	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}