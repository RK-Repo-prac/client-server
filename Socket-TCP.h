#ifndef SOCKET_TCP
#define SOCKET_TCP

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>

class Socket_TCP {
private:
    struct sockaddr_in sock_addr_;
    short IP_FAMILY_;
    u_short sin_port_;
    u_long sin_addr_;
    int client_connections_;
    int sockfd_;
    fd_set fr_, fw_, fe_;

public:
    Socket_TCP(short IP_FAMILY, u_short sin_port, u_long sin_addr);
    int sock_listen(int max_connections);
    int sock_connect(int &sockfd, sockaddr_in &serveraddr);
    int sock_select(timeval &time_value);
    bool fd_isset();
    void close_sock() { closesocket(sockfd_); }
    int accept_sock();
    int receive();
    int get_sockfd() const { return sockfd_; }
};

#endif 