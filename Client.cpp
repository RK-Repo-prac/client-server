#pragma comment(lib, "Ws2_32.lib")
#include "Socket-TCP.h"
#include <iostream>
#include <string>

#define PORT 8888
#define BUFFER_SIZE 1024

int main() {
    try {
        Socket_TCP client_socket(AF_INET, PORT, INADDR_ANY);
        
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(7777);
        
        if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
            std::cerr << "Invalid address or address not supported" << std::endl;
            client_socket.close_sock();
            WSACleanup();
            return 1;
        }
        
        int sockfd = client_socket.get_sockfd();
        int connect_result = client_socket.sock_connect(sockfd, server_addr);
        if (connect_result < 0) {
            std::cerr << "Failed to connect to server" << std::endl;
            return 1;
        }
        
        std::cout << "Connected to server at 127.0.0.1:7777" << std::endl;
        
        int received = client_socket.receive();
        if (received <= 0) {
            std::cerr << "Failed to receive initial message" << std::endl;
        }
        
        char buffer[BUFFER_SIZE];
        std::string user_input;
        
        while (true) {
            std::cout << "\nEnter message (or 'exit' to quit): ";
            std::getline(std::cin, user_input);
            
            if (user_input == "exit") {
                break;
            }
            
            int send_result = send(sockfd, user_input.c_str(), user_input.length(), 0);
            if (send_result == SOCKET_ERROR) {
                std::cerr << "Failed to send message, error: " << WSAGetLastError() << std::endl;
                break;
            }
            
            memset(buffer, 0, BUFFER_SIZE);
            int recv_result = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
            
            if (recv_result > 0) {
                buffer[recv_result] = '\0';
                std::cout << "Server response: " << buffer << std::endl;
            } else if (recv_result == 0) {
                std::cout << "Server closed the connection" << std::endl;
                break;
            } else {
                std::cerr << "Error receiving data: " << WSAGetLastError() << std::endl;
                break;
            }
        }
        
        client_socket.close_sock();
        WSACleanup();
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        WSACleanup();
        return 1;
    }
    
    return 0;
}