#ifndef TIN_TCPHANDLER
#define TIN_TCPHANDLER


#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>

#include "Constants.h"
#include "Resource.h"

class TCPConnector {


    public:
        TCPConnector(int socket, unsigned short port, const char* address) {

            remote_addr.sin_port = htons(port);

            remote_addr.sin_addr.s_addr = inet_addr(address);
            remote_addr.sin_family = AF_INET;
            memset(&(remote_addr.sin_zero), '\0', 8);

            // std::cout << remote_addr.sin_port << " " << remote_addr.sin_addr.s_addr << " " << address << std::endl;

            this->socket = socket;
            this->port = port;
            this->address = address;
            // int status = connect(socket, &remote_addr,  socklen_t addrlen);
        }

        TCPConnector(int socket, const struct sockaddr_in &addr) {
            this->socket = socket;
            this->remote_addr = addr;
        }

        ~TCPConnector() {}


        int setupClient() {
            int error;
            // std::cout << "TCP:: setting up client\n";
            if ( (error = connect(this->socket, (struct sockaddr *)&remote_addr, sizeof(remote_addr))) < 0 ) {
                printError();
                return -1;
            }
            // std::cout << "TCP:: set client with " << error << "\n";
                return 0;
        }


        int serverListen() {
            if( listen(socket, 10) < 0 ) {
                printError();
                return -1;
            }
            if (DEBUG_LOG) std::cout << "[I] TCP:: Listening on " << remote_addr.sin_addr.s_addr << ":" << remote_addr.sin_port << "\n";
            return 0;
        }


        int serverAccept() {
            //wyslac żądanie o wątek na obsłużenie wysyłania pliku czy coś
            int receiver;
            socklen_t len = sizeof(remote_addr);

            if ( (receiver = accept(socket, (struct sockaddr *)&remote_addr, &len)) < 0 ) {
                printError();
                return -1;
            }

            //zwrocenie deskryptora socketu do wysylania danych
            return receiver;
        }


        int sendData(int sockfd, void *ptr, unsigned long long data_size) {
            int bytes_sent = 0;
            void* data_ptr = ptr;
            // std::cout << "TCP: sendData() @" << sockfd << " " << ptr << " " << data_ptr << " " << data_size << "\n";
            int error_code;
            unsigned int error_code_size = sizeof(error_code);
            int sockopt = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error_code, &error_code_size);
            // std::cout << error_code << " " << error_code_size << " " << sockopt << "\n";
            // std::cout << strerror(error_code) << "\n";

            while( (bytes_sent = send(sockfd, data_ptr, data_size, 0) ) > 0) {

                std::cout << "[I] Sent " << bytes_sent << " bytes\n";
                data_size -= bytes_sent;

                if (data_size < 1) {
                    std::cout << "[I] Exiting sendData()\n";
                    return 0;
                }
                data_ptr = static_cast<char*>(data_ptr) + bytes_sent;
            }


            if (bytes_sent < 0) {
                printError();
                return -1;
            } else {
                return 0;
            }
        }


        int receiveData(int fd, void *ptr, unsigned long long data_size) {
            int bytes_received = 0;
            void* data_ptr = ptr;
            int recv_fd;
            if (fd != 0)
                recv_fd = fd;
            else
                recv_fd = this->socket;

            // std::cout << "TCP: receiveData() @" << recv_fd << " " << ptr << " " << data_ptr << " " << data_size << "\n";
            // std::cout << ptr << " " << data_ptr << "\n";
            while( (bytes_received = recv(recv_fd, data_ptr, data_size, 0)) > 0) {

                std::cout << "[I] Received " << bytes_received << " bytes\n";
                data_size -= bytes_received;

                if (data_size < 1) {
                    std::cout << "[I] Exiting receiveData()\n";

                    return 0;
                }

                data_ptr = static_cast<char*>(data_ptr) + bytes_received;
            }


            if (bytes_received < 0) {
                printError();
                return -1;
            } else {
                return 0;
            }
        }
        
    private:
        int socket;
        unsigned short port;
        const char* address;
        sockaddr_in remote_addr;

        void printError() {
            std::cout << "[ERR] " << strerror(errno) << "\n";
        }

};




#endif