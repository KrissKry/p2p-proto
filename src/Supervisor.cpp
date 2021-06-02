//
// Created by Michal Wiszenko on 30/05/2021.
//

#include <iostream>
#include <thread>
#include "../include/Supervisor.h"

Supervisor::Supervisor() {
    fileHandler = new FileHandler();
    network_handler = new NetworkHandler(tcp_upflow, tcp_downflow, udp_upflow, udp_downflow);
}

Supervisor::~Supervisor() { delete fileHandler; }

void Supervisor::run() {
    // start network connector
    std::thread tcpQueueListener(&Supervisor::tcpQueueListener, this);
    std::thread udpQueueListener(&Supervisor::udpQueueListener, this);

    int res = network_handler->createNewTCPServer();
    if(res == 0) {
        std::thread tcpServer(&NetworkHandler::handleTCPServer, network_handler, 0);
        tcpServer.detach();
    }

    std::thread udpServer(&NetworkHandler::udpServerRun, network_handler);
    udpServer.detach();

    tcpQueueListener.join();
    udpQueueListener.join();
}

void Supervisor::tcpQueueListener() {
    std::pair<int, ProtoPacket> message;
    while(shouldRun) {
        tcp_upflow.pop(message);
        std::cout<<message.second.command<<std::endl;
    }
}

void Supervisor::udpQueueListener() {
    ProtoPacket message;
    while(shouldRun) {
        udp_upflow.pop(message);
        switch (message.command)
        {
            case Commands::CREATE:
                createFile(message.header);
                break;
            case Commands::DELETE:
                deleteFile(message.header);
                break;
            case Commands::GET_INFO:
                getInfo();
                break;
            default:
                break;
        }
    }
}

void Supervisor::broadcastCreate(ResourceHeader resourceHeader) {
    ProtoPacket protoPacket;
    protoPacket.command = Commands::CREATE;
    protoPacket.header = resourceHeader;
    udp_downflow.push(protoPacket);
}

void Supervisor::broadcastDelete(ResourceHeader resourceHeader) {
    ProtoPacket protoPacket;
    protoPacket.command = Commands::DELETE;
    protoPacket.header = resourceHeader;
    udp_downflow.push(protoPacket);
}

void Supervisor::broadcastGetInfo(ResourceHeader resourceHeader) {
    ProtoPacket protoPacket;
    protoPacket.command = Commands::GET_INFO;
    protoPacket.header = resourceHeader;
    udp_downflow.push(protoPacket);
}

void Supervisor::getInfo() {
    std::vector<ResourceHeader> files = fileHandler->getNetFileList();
    // tcp send
}

void Supervisor::createFile(ResourceHeader header) {
    fileHandler->NewFileInfo(header, "123");
}

void Supervisor::deleteFile(ResourceHeader resHeader) {
    fileHandler->deleteFromNetList(resHeader);
}

int Supervisor::createFile(const std::string& path, const std::string& name) {
    int res = fileHandler->AddFile(path.c_str(), name.c_str(), "123");
    ResourceHeader header{};
    if(res == 0) {
        broadcastCreate(header);
        return 0;
    }
    return -1;
}

int Supervisor::downloadFile(const std::string& name) {
//    int res = fileHandler->getFile(name);
//    if(res == 0) {
//        ProtoPacket packet{};
//        packet.command = 3;
//        tcp_downflow.push(packet);
//        return 0;
//    }
//    return -1;
}

int Supervisor::deleteFile(const std::string& name) {
    ResourceHeader header{};
    strcpy(header.name, name.c_str());
    int res = fileHandler->deleteownFile(header);
    if(res == 0) {
        broadcastDelete(header);
        return 0;
    }
    return -1;
}

std::vector<Resource> Supervisor::listDisk() {
    return fileHandler->getOwnFileList();
}

void Supervisor::cleanUp() {
    shouldRun = false;
}
