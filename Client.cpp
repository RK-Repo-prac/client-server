#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 8888

struct sockaddr_in clientaddr;

struct sockaddr_in serveraddr;

int main(){

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    std::cerr << "WSAStartup failed" << std::endl;
    exit(EXIT_FAILURE);
    }

   int clientsockfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);


   if(clientsockfd<0){
    std::cout<<"The socket Initialization is not successful"<<std::endl;
    closesocket(clientsockfd);
    WSACleanup();
    exit(EXIT_FAILURE);
   }
   else{
    std::cout<< "The socket is created successfully"<<std::endl;
   }
   
   memset(&clientaddr, 0, sizeof(clientaddr));
   clientaddr.sin_addr.s_addr=INADDR_ANY;
   clientaddr.sin_family=AF_INET;
   clientaddr.sin_port=htons(PORT);
   int option=1;
   int sockoptres=setsockopt(clientsockfd, SOL_SOCKET, SO_REUSEADDR,(const char*) &option, sizeof(option)); //used to set socket Options.
    if (sockoptres < 0)
    {
        std::cout<<"Unable to Set our Socket Option"<<std::endl;
        closesocket(clientsockfd);
        WSACleanup();
        exit(EXIT_FAILURE);

    }
    else{
        std::cout<<"SockOption is set successfully :"<<std::endl;

    }
   
   int bindres=bind(clientsockfd,(sockaddr*)&clientaddr, sizeof(clientaddr));
   if (bindres < 0)
    {
        std::cout<<"Unable to bind the port number to our socket"<<std::endl;
        closesocket(clientsockfd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else{
        std::cout<<"Binding is successful"<<std::endl;

    }

    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(7777);

    int connectRes=connect(clientsockfd,(sockaddr*)&serveraddr,sizeof(serveraddr));
    if (connectRes < 0)
    {
        std::cout<<"Unable to connect to Server"<<std::endl;
        closesocket(clientsockfd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else{
        char msg[255];
        int len = recv(clientsockfd, msg, 254, 0); 

        if (len > 0) {
            msg[len] = '\0';
            std::cout << "Received " << len << " bytes: " << msg << std::endl;
        } else if (len == 0) {
            std::cout << "Server disconnected" << std::endl;
        } else {
            std::cout << "Error receiving data" << std::endl;
        }

    }
   closesocket(clientsockfd);
   WSACleanup();
   return 0;
}