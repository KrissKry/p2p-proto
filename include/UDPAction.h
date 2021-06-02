#ifndef UDPAction_H
#define UDPAction_H

#include <iostream>
#include "Resource.h"
#include "SyncedDeque.h"

class UDPAction
{
public:
    UDPAction(SyncedDeque<ProtoPacket> &udp_up,
              SyncedDeque<ProtoPacket> &udp_down) : udp_upflow(udp_up),
                                                    udp_downflow(udp_down)
    {
    }

    void broadcastCreate(ResourceHeader resourceHeader)
    {
        ProtoPacket protoPacket;
        protoPacket.command = Commands::CREATE;
        protoPacket.header = resourceHeader;
        udp_downflow.push(protoPacket);
    }
    void broadcastDelete(ResourceHeader resourceHeader)
    {
        ProtoPacket protoPacket;
        protoPacket.command = Commands::DELETE;
        protoPacket.header = resourceHeader;
        udp_downflow.push(protoPacket);
    }
    void broadcastGetInfo(ResourceHeader resourceHeader)
    {
        ProtoPacket protoPacket;
        protoPacket.command = Commands::GET_INFO;
        protoPacket.header = resourceHeader;
        udp_downflow.push(protoPacket);
    }
    void listener()
    {
        while (true)
        {
            ProtoPacket protoPacket;

            switch (protoPacket.command)
            {
            case Commands::CREATE:
                //fileHandler
                break;
            case Commands::DELETE:
                //fileHandler
                break;
            case Commands::GET_INFO:
                //fileHandler
                break;
            default:
                break;
            }
        }
    }

private:
    SyncedDeque<ProtoPacket> &udp_upflow;
    SyncedDeque<ProtoPacket> &udp_downflow;
};

#endif