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
#include <sys/select.h>
#include "Resource.h"
#include "SyncedDeque.h"

class UDPClient
{
private:
    const int UDP_PORT = 10000;

public:
    void broadcast(ProtoPacket protoPacket)
    {

        int socketDesc = socket(AF_INET, SOCK_DGRAM, 0);
        if (DEBUG_LOG) std::cout << "socketDesc " << socketDesc << std::endl;

        if (socketDesc < 0)
        {
            perror("sock error");
        }

        //set socket options to broadcast
        int optval = 1;
        int options = setsockopt(socketDesc, SOL_SOCKET, SO_BROADCAST, (char *)&optval, sizeof(optval));

        if (options == -1)
        {
            perror("UDP broadcast:setsockopt error");
        }

        //set address structure
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(struct sockaddr_in));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
        addr.sin_port = htons(UDP_PORT);

        //send data
        // int feed = sendto(socketDesc, &protoPacket, sizeof(protoPacket), 0, (struct sockaddr *)&addr, sizeof(addr));
        int feed = sendto(socketDesc, &protoPacket, sizeof(protoPacket), 0, (struct sockaddr *)&addr, sizeof(addr));

        if (feed < 0)
        {
            std::cout << feed << std::endl;
            perror("UDP broadcast:sendto");
        }
        //close(socketDesc);
    }
    void server(SyncedDeque<std::pair<struct in_addr, ProtoPacket>> &udp_upflow, std::atomic_bool &stop)
    {

        //create udp socket
        int socketDesc = socket(AF_INET, SOCK_DGRAM, 0);
        if (socketDesc < 0)
        {
            if (DEBUG_LOG) std::cout << "socketDesc" << std::endl;
            perror("sock error");
        }
        //set address struct
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(struct sockaddr_in));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(UDP_PORT);

        //bind socket to address
        int bindFeed = bind(socketDesc, (struct sockaddr *)&addr, sizeof(addr));
        if (bindFeed < 0)
        {
            perror("bind error");
            exit(1);
        }
        while (!stop)
        {
            ProtoPacket protoPacket;
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
                    int receivedBytes = recvfrom(socketDesc, &protoPacket, sizeof(protoPacket), 0, (struct sockaddr *)&clientAddr, &addrLen);

                    if (receivedBytes > 0)
                    {
                        udp_upflow.push(std::pair<struct in_addr, ProtoPacket>(clientAddr.sin_addr, protoPacket));
                    }
                }
            }
        }
    }
};

#endif