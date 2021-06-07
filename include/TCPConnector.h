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
        /* used when creating a client */
        explicit TCPConnector(int socket, unsigned short port, const char* address);

        /* used when creating a server*/
        explicit TCPConnector(int socket, const struct sockaddr_in &addr);

        ~TCPConnector() {}

        /* connects() client to remote addr */ 
        int setupClient();

        /* begins tcp server listening */
        int serverListen();

        /*
        *   accept incoming connection on server socket from any IP
        *   returns file descriptor for the connection or -1 on error
        */
        int serverAccept();

        /*
        *   send >data_size< bytes from memory @ptr on >sockfd<
        *   returns 0 on success, -1 on error
        */
        int sendData(int sockfd, void *ptr, unsigned long long data_size);

        /*
        *   receive >data_size< bytes into memory @ptr from socket >fd<
        */
        int receiveData(int fd, void *ptr, unsigned long long data_size);
        

    private:
        int socket;
        unsigned short port;
        const char* address;
        sockaddr_in addr;

        void printError() {
            std::cout << "[ERR] " << strerror(errno) << "\n";
        }

};




#endif