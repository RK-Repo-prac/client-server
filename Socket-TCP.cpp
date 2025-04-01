#pragma comment(lib, "Ws2_32.lib")

#include "Socket-TCP.h"
#include <ws2tcpip.h>

Socket_TCP::Socket_TCP(short IP_FAMILY, u_short sin_port, u_long sin_addr)
{   
    WSADATA wsaData;
    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaResult != 0) {
        std::cerr << "WSAStartup failed with error: " << wsaResult << std::endl;
        exit(EXIT_FAILURE);
    }

    IP_FAMILY_ = IP_FAMILY;
    sin_port_ = sin_port;
    sin_addr_ = sin_addr;

    sockfd_ = socket(IP_FAMILY_, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd_ == INVALID_SOCKET) {
        std::cerr << "Socket initialization failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    
    std::cout << "The socket was initialized successfully with Socket descriptor: " << sockfd_ << std::endl;

    memset(&sock_addr_, 0, sizeof(sock_addr_));
    sock_addr_.sin_family = IP_FAMILY_;
    sock_addr_.sin_port = htons(sin_port_);
    sock_addr_.sin_addr.s_addr = sin_addr_; 
    
    int option = 1;
    int nRet = setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, (const char*)&option, sizeof(option));
    if (nRet == SOCKET_ERROR) {
        std::cerr << "Unable to set socket option with error: " << WSAGetLastError() << std::endl;
        closesocket(sockfd_);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    
    std::cout << "Socket option is set successfully" << std::endl;

    int bindResult = bind(sockfd_, (sockaddr*)&sock_addr_, sizeof(sock_addr_));
    if (bindResult == SOCKET_ERROR) {
        std::cerr << "Unable to bind the port number to socket with error: " << WSAGetLastError() << std::endl;
        closesocket(sockfd_);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    
    std::cout << "Binding is successful" << std::endl;
}

int Socket_TCP::sock_listen(int client_connections) {
    client_connections_ = client_connections;
    int listenRet = listen(sockfd_, client_connections);
    
    if (listenRet == SOCKET_ERROR) {
        std::cerr << "Unable to listen on the socket " << WSAGetLastError() << std::endl;
        closesocket(sockfd_);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    
    std::cout << "Server is ready to receive connections" << std::endl;
    return listenRet;
}

int Socket_TCP::sock_select(timeval &time_value) {
    FD_ZERO(&fr_);
    FD_ZERO(&fw_);
    FD_ZERO(&fe_);
    
    FD_SET(sockfd_, &fr_);
    FD_SET(sockfd_, &fe_);
    
    return select(sockfd_ + 1, &fr_, NULL, &fe_, &time_value);
}

int Socket_TCP::accept_sock() {
    sockaddr_in client_details;
    int client_addrlen = sizeof(client_details);
    
    int client_sock = accept(sockfd_, (sockaddr*)&client_details, &client_addrlen);
    if (client_sock == INVALID_SOCKET) {
        std::cerr << "Accept failed " << WSAGetLastError() << std::endl;
        return -1;
    }
    
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_details.sin_addr), ip_str, INET_ADDRSTRLEN);
    
    std::cout << "Client connected from IP: " << ip_str << std::endl;
    std::cout << "Client port number: " << ntohs(client_details.sin_port) << std::endl;
    
    return client_sock;
}

int Socket_TCP::sock_connect(int &sockfd, sockaddr_in &serveraddr) {
    int connectRes = connect(sockfd, (sockaddr*)&serveraddr, sizeof(serveraddr));
    
    if (connectRes == SOCKET_ERROR) {
        std::cerr << "Unable to connect to server with error: " << WSAGetLastError() << std::endl;
        closesocket(sockfd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    
    std::cout << "Connection was established successfully" << std::endl;
    return connectRes;
}

int Socket_TCP::receive() {
    char msg[1024];
    int len = recv(sockfd_, msg, sizeof(msg) - 1, 0);
    
    if (len > 0) {
        msg[len] = '\0';
        std::cout << "Received " << len << " bytes: " << msg << std::endl;
        return len;
    } else if (len == 0) {
        std::cout << "Connection closed by peer" << std::endl;
        return 0;
    } else {
        std::cerr << "Error receiving data: " << WSAGetLastError() << std::endl;
        return -1;
    }
}

bool Socket_TCP::fd_isset() {
    return FD_ISSET(sockfd_, &fr_);
}