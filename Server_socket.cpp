#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

/*
struct sockaddr_in {
	short	sin_family;
	u_short	sin_port;
	struct in_addr	sin_addr;
	char	sin_zero[8];
};
*/

struct pollfd ;

struct sockaddr_in socinit;
fd_set fr,fw,fe;

int main() {

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    std::cerr << "WSAStartup failed" << std::endl;
    return 1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    std::cout<<"The file descriptor assigned to our socket is: "<<sockfd<<std::endl;
    if (sockfd > 0) {
        std::cout << "The socket was initialized successfully" << std::endl;
    } else {
        std::cerr << "Socket initialization failed "<< std::endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    socinit.sin_family=AF_INET;
    socinit.sin_port=htons(7777);
    socinit.sin_addr.s_addr =INADDR_ANY;
    int option = 1;
    int nRet=setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,(const char*) &option, sizeof(option)); //used to set socket Options.
    if (nRet < 0)
    {
        std::cout<<"Unable to Set our Socket Option"<<std::endl;
        closesocket(sockfd);
        WSACleanup();
        exit(EXIT_FAILURE);

    }
    else{
        std::cout<<"SockOption is set successfully :"<<std::endl;

    }
    int bindResult = bind(sockfd, (sockaddr*)&socinit, sizeof(socinit)); //question
    if (bindResult < 0)
    {
        std::cout<<"Unable to bind the port number to our socket"<<std::endl;
        closesocket(sockfd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else{
        std::cout<<"Binding is successful :"<<std::endl;

    }

    int listenRet = listen(sockfd,4);  // we will only get the incoming requests from this function as the output. this function is not used to recieve any messages
    if(listenRet <0)
    {
        std::cout<<"We are unable to listen to the incoming messages from the socket "<<std::endl;
        closesocket(sockfd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else{
    std::cout<<"We are ready to receive messages from the socket "<<std::endl;

    }

    struct timeval time_value;
    time_value.tv_sec=1;
    time_value.tv_usec=0;
    while (1)
    {
        FD_ZERO(&fr);
        FD_ZERO(&fw);
        FD_ZERO(&fe);
        FD_SET(sockfd,&fr);
        FD_SET(sockfd,&fe);
        int selectresult;
        selectresult=select((sockfd+1), &fr, &fw, &fe , &time_value); // Other way to do is pollfd
        if(selectresult > 0){
            
            std::cout<<"Received new connections"<<std::endl;
            break;
        }else if (selectresult == 0)
        {
            std::cout<<"No New Connections"<<std::endl;  
        }
        else{
            std::cout<< "Unable" ;
            closesocket(sockfd);
            WSACleanup();
            exit(EXIT_FAILURE);
        }
        
        
    }
    
    closesocket(sockfd);
    WSACleanup();
    return 0;
}
