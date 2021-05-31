#ifndef UDPClient_H
#define UDPClient_H

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>
#include <string>
#include <unistd.h> //sleep
class UDPClient
{
public:
    void greet()
    {
        std::cout << "GREETINGS" << std::endl;
    }
    void broadcast(const char *data)
    {
        const char *message = data;

        //create udp socket
        int socketDesc = socket(AF_INET, SOCK_DGRAM, 0);
        std::cout << "socketDesc " << socketDesc << std::endl;
        //TODO: catch exceptions :)
        if (socketDesc < 0)
        {
            std::cout << "socketDesc" << std::endl;
            perror("sock error");
        }

        //set socket options to broadcast
        int optval = 1;
        int options = setsockopt(socketDesc, SOL_SOCKET, SO_BROADCAST, (char *)&optval, sizeof(optval));

        if (options == -1)
        {
            perror("setsockopt error");
        }

        //set address structure
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(struct sockaddr_in));
        addr.sin_family = AF_INET;
        // addr.sin_addr.s_addr = inet_addr("192.168.0.255");
        addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
        addr.sin_port = htons(10000);

        //sending data
        int feed = sendto(socketDesc, message, strlen(message), 0, (struct sockaddr *)&addr, sizeof(addr));

        if (feed < 0)
        {
            std::cout << feed << std::endl;
            perror("sendto");
        }
    }
    void serwer()
    {

        //create udp socket
        int socketDesc = socket(AF_INET, SOCK_DGRAM, 0);
        if (socketDesc < 0)
        {
            std::cout << "socketDesc" << std::endl;
            perror("sock error");
        }
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(struct sockaddr_in));
        addr.sin_family = AF_INET;
        // addr.sin_addr.s_addr = inet_addr("192.168.0.255");
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(10000);

        //bind socket to address
        int bindFeed = bind(socketDesc, (struct sockaddr *)&addr, sizeof(addr));
        if (bindFeed < 0)
        {
            perror("bind error");
            exit(1);
        }
        while (1)
        {
            char messageBuffer[1024];
            fd_set readfds;
            struct sockaddr_in clientAddr;
            socklen_t addrLen = sizeof(clientAddr);
            FD_ZERO(&readfds);
            FD_SET(socketDesc, &readfds);

            int selectFeed = select(socketDesc + 1, &readfds, NULL, NULL, 0);
            //number of fdesc actions
            if (selectFeed > 0)
            {
                if (FD_ISSET(socketDesc, &readfds))
                {
                    int receivedBytes = recvfrom(socketDesc, messageBuffer, sizeof(messageBuffer), 0, (struct sockaddr *)&clientAddr, &addrLen);

                    if (receivedBytes > 0)
                    {
                        std::cout << messageBuffer << std::endl;
                    }
                }
            }
        }
    }
};

#endif