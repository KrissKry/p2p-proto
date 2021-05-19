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
        int fileDesc = socket(AF_INET, SOCK_DGRAM, 0);
        std::cout << "filedesc " << fileDesc << std::endl;
        //TODO: catch exceptions :)
        if (fileDesc < 0)
        {
            std::cout << "fileDesc" << std::endl;
            perror("sock error");
        }

        int optval = 1;
        int options = setsockopt(fileDesc, SOL_SOCKET, SO_BROADCAST, (char *)&optval, sizeof(optval));

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
        int feed = sendto(fileDesc, message, strlen(message), 0, (struct sockaddr *)&addr, sizeof(addr));

        if (feed < 0)
        {
            std::cout << feed << std::endl;
            perror("sendto");
        }
    }
    void serwer()
    {
    }
};

#endif