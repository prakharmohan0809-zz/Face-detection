/*  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.

  By downloading, copying, installing or using the software you agree to this license.
  If you do not agree to this license, do not download, install,
  copy or use the software.
                        Intel License Agreement
                For Open Source Computer Vision Library

 Copyright (C) 2000, Intel Corporation, all rights reserved.
 Third party copyrights are property of their respective owners.
 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:
   * Redistribution's of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
   * Redistribution's in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
   * The name of Intel Corporation may not be used to endorse or promote products
     derived from this software without specific prior written permission.
 This software is provided by the copyright holders and contributors "as is" and
 any express or implied warranties, including, but not limited to, the implied
 warranties of merchantability and fitness for a particular purpose are disclaimed.
 In no event shall the Intel Corporation or contributors be liable for any direct,
 indirect, incidental, special, exemplary, or consequential damages
 (including, but not limited to, procurement of substitute goods or services;
 loss of use, data, or profits; or business interruption) however caused
 and on any theory of liability, whether in contract, strict liability,
 or tort (including negligence or otherwise) arising in any way out of
 the use of this software, even if advised of the possibility of such damage.
*/
#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include<iostream>
#include<sstream>

using namespace std;
using namespace cv;

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

#pragma comment(lib, "Ws2_32.lib")

void detectAndDisplay(Mat frame);

struct addrinfo *result = NULL, *ptr = NULL, hints;

cv::CascadeClassifier face_cascade;
String face_cascade_name="C:\\Documents and Settings\\prakar\\Desktop\\opencv\\data\\haarcascades\\haarcascade_frontalface_alt2.xml";
cv::RNG rng(12345);

int x,y;

int main()
{

	WSADATA wsadata;
	char* sendbuf;
	int iSendResult;
	int iResult;
	
	if(!face_cascade.load(face_cascade_name))
	{
		printf("Error loading cascade classifier\n");
		getchar();
		return -1;
	}

	std::cout<<"Starting the server"<<endl;
	iResult=WSAStartup(MAKEWORD(2,2),&wsadata);
	
	if(iResult!=0)
	{
		std::cout<<"WSAStartup failed"<<std::endl;
		getchar();
		exit(1);
	}
	

	ZeroMemory(&hints, sizeof (hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		std::cout<<"getaddrinfo failed:"<<iResult<<std::endl;
		getchar();
		WSACleanup();
		exit(1);
	}
	
	SOCKET ListenSocket=INVALID_SOCKET;
	
	ListenSocket=socket(result->ai_family,result->ai_socktype,result->ai_protocol);

	if(ListenSocket==INVALID_SOCKET)
	{
		std::cout<<"Error at socket():"<<WSAGetLastError()<<std::endl;
		freeaddrinfo(result);
		getchar();
		WSACleanup();
		exit(1);
	}
	
	iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
        
	if (iResult == SOCKET_ERROR)
	{	

		std::cout<<"Bind failed with error:"<<WSAGetLastError()<<std::endl;
        getchar();
		closesocket(ListenSocket);
        freeaddrinfo(result);
    	WSACleanup();
    }
	
	freeaddrinfo(result);

	if (listen(ListenSocket,SOMAXCONN)==SOCKET_ERROR)
	{
		std::cout<<"Listen failed with error:"<<WSAGetLastError();
		getchar();
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}
	
	SOCKET ClientSocket;

	ClientSocket = accept(ListenSocket, NULL , NULL); //error??
	
	if (ClientSocket == INVALID_SOCKET)
	{
		std::cout<<"accept failed:"<<WSAGetLastError();
		getchar();
		closesocket(ListenSocket);
		WSACleanup();
		exit(1);
	}
	
	VideoCapture cap(0);
	if(!cap.isOpened())
	{
		cout<<"Camera is not connected"<<endl;
		getchar();
	}
	Mat edges;
    namedWindow("Camera Feed",1);
    for(;;)
    {
        Mat frame;
        cap >> frame;
		imshow("Camera Feed", frame);
		if(!frame.empty())
			detectAndDisplay(frame);
		else
			cout<<"No frame as input"<<endl;
		int c=waitKey(10);
		if(c==27)
		exit(1);
		
		sendbuf=(char *) &x;
		
		iResult = send(ClientSocket,sendbuf,(int)strlen(sendbuf),0);
		
		if (iResult == SOCKET_ERROR) 
		{
			std::cout<<"send failed from server";
			getchar();
			closesocket(ListenSocket);
			WSACleanup();
			exit(1);
		}
	}

		std::cout<<"Closing socket"<<std::endl;
		getchar();
		closesocket(ListenSocket);
	    WSACleanup();

	
	return 0;
}
void detectAndDisplay(cv::Mat frame)
{	
	cv::Mat im1,im2;
	std::vector<Rect> faces;
	Mat frame_gray;
	cv::cvtColor(frame,frame_gray,CV_BGR2GRAY);
	face_cascade.detectMultiScale(frame_gray,faces,1.1,2,0|CV_HAAR_SCALE_IMAGE,cv::Size(30,30));
	
	
    for( int i = 0; i < faces.size(); i++ )
	{
    rectangle(frame,Point(faces[i].x,faces[i].y),Point(faces[i].x+faces[i].width,faces[i].y+faces[i].height),Scalar(255,0,0),4,8,0);
    Mat faceROI = frame_gray(faces[i]);
	x=((faces[i].x+faces[i].width)-faces[i].x);
	y=((faces[i].y+faces[i].height)-faces[i].y);
	}
	
	imshow("Camera Feed",frame);
}