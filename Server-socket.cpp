#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>
#include "Socket-TCP.h"
#pragma comment(lib, "Ws2_32.lib")

#define SOCKET_MAX 5
#define PORT 7777
#define BUFFER_SIZE 1024

int socketfd_array[SOCKET_MAX];
struct sockaddr_in client_details;
char buffer[BUFFER_SIZE];

void acceptconn(Socket_TCP& socket) {
    if (socket.fd_isset()) {
        int client_sock = socket.accept_sock();
        if (client_sock < 0) {
            std::cerr << "Failed to accept connection" << std::endl;
            return;
        }

        bool added = false;
        for (int i = 0; i < SOCKET_MAX; i++) {
            if (socketfd_array[i] == 0) {
                socketfd_array[i] = client_sock;
                added = true;
                std::cout << "New client connected with socket: " << client_sock << std::endl;
                const char* msg = "Connection established successfully!";
                send(client_sock, msg, strlen(msg), 0);
                break; 
            }
        }
        
        if (!added) {
            std::cout << "The maximum number of connections have already been reached" << std::endl;
            const char* msg = "Server full";
            send(client_sock, msg, strlen(msg), 0);
            closesocket(client_sock);
        }
    }
}

void process_client_messages() {
    for (int i = 0; i < SOCKET_MAX; i++) {
        if (socketfd_array[i] != 0) {
            fd_set read_set;
            FD_ZERO(&read_set);
            FD_SET(socketfd_array[i], &read_set);
            
            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 0;
            
            int result = select(0, &read_set, NULL, NULL, &tv);
            
            if (result > 0) {
                memset(buffer, 0, BUFFER_SIZE);
                int bytes_read = recv(socketfd_array[i], buffer, BUFFER_SIZE - 1, 0);
                
                if (bytes_read > 0) {
                    buffer[bytes_read] = '\0';
                    std::cout << "Received from client " << i << ": " << buffer << std::endl;
                    
                    send(socketfd_array[i], buffer, bytes_read, 0);
                } else if (bytes_read == 0) {
                    std::cout << "Client " << i << " disconnected" << std::endl;
                    closesocket(socketfd_array[i]);
                    socketfd_array[i] = 0;
                } else {
                    std::cerr << "Error receiving data from client " << i << ": " << WSAGetLastError() << std::endl;
                    closesocket(socketfd_array[i]);
                    socketfd_array[i] = 0;
                }
            }
        }
    }
}

int main() {
    memset(socketfd_array, 0, sizeof(socketfd_array));
    
    Socket_TCP server_sock(AF_INET, PORT, INADDR_ANY);
    
    if (server_sock.sock_listen(SOCKET_MAX) < 0) {
        std::cerr << "Failed to set listen socket" << std::endl;
        WSACleanup();
        return 1;
    }
    
    std::cout << "Server started on port " << PORT << std::endl;
    std::cout << "Waiting for connections..." << std::endl;

    struct timeval time_value;
    time_value.tv_sec = 1;
    time_value.tv_usec = 0;
    
    while (1) {
        int selectresult = server_sock.sock_select(time_value);
        
        if (selectresult > 0) {
            acceptconn(server_sock);
        } else if (selectresult < 0) {
            std::cerr << "Select error: " << WSAGetLastError() << std::endl;
            break;
        }
        
        process_client_messages();
    }
    
    for (int i = 0; i < SOCKET_MAX; i++) {
        if (socketfd_array[i] != 0) {
            closesocket(socketfd_array[i]);
        }
    }
    
    server_sock.close_sock();
    WSACleanup();
    return 0;
}