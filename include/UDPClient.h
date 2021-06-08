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
    int broadcast(ProtoPacket protoPacket);

    int server(SyncedDeque<std::pair<struct in_addr, ProtoPacket>> &udp_upflow, std::atomic_bool &stop);
};

#endif