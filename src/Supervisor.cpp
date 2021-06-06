//
// Created by Michal Wiszenko on 30/05/2021.
//

#include <iostream>
#include <thread>
#include "../include/Supervisor.h"

Supervisor::Supervisor() {
    fileHandler = new FileHandler();
    tcp_downflow.setStopper(this->stop);
    tcp_upflow.setStopper(this->stop);
    udp_downflow.setStopper(this->stop);
    udp_upflow.setStopper(this->stop);
    networkHandler = new NetworkHandler(tcp_upflow, tcp_downflow, udp_upflow, udp_downflow);
}

Supervisor::~Supervisor() { delete fileHandler; delete networkHandler; }

void Supervisor::run() {
    // start network connector
    std::thread tcpQueueListener(&Supervisor::tcpQueueListener, this);
    std::thread udpQueueListener(&Supervisor::udpQueueListener, this);

    int res = networkHandler->createNewTCPServer();
    if(res == 0) {
        std::thread tcpServer(&NetworkHandler::handleTCPServer, networkHandler, 0);
        tcpServer.detach();
    }

    std::thread udpServer(&NetworkHandler::udpServerRun, networkHandler);
    udpServer.detach();

    tcpQueueListener.join();
    udpQueueListener.join();
}

void Supervisor::tcpQueueListener() {
    std::pair<int, ProtoPacket> message;
    while(shouldRun) {
        tcp_upflow.pop(message);
        Resource res = {};
        res.header = message.second.header;
        res.data = message.second.data;
        switch (message.second.command)
        {
            case Commands::DOWNLOAD:
                handleDownload(res.header);
                break;
            case Commands::UPLOAD:
                handleUpload(res);
                break;
            default:
                break;
        }
    }
}

void Supervisor::udpQueueListener()
{
    std::pair<struct in_addr, ProtoPacket> message;
    while (shouldRun)
    {
        udp_upflow.pop(message);
        switch (message.second.command)
        {
            case Commands::CREATE:
                handleCreate(message.second.header);
                break;
            case Commands::DELETE:
                handleDelete(message.second.header);
                break;
            case Commands::GET_INFO:
                handleGetInfo();
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

void Supervisor::sendDownload(ResourceHeader resourceHeader) {
    ProtoPacket protoPacket;
    protoPacket.command = Commands::DOWNLOAD;
    protoPacket.header = resourceHeader;
    const std::pair message(1, protoPacket);
    tcp_downflow.push(message);
}

void Supervisor::sendUpload(const Resource& res) {
    ProtoPacket protoPacket;
    protoPacket.command = Commands::UPLOAD;
    protoPacket.header = res.header;
    protoPacket.data = res.data;
    const std::pair message(1, protoPacket);
    tcp_downflow.push(message);
}

void Supervisor::handleGetInfo() {
    std::vector<ResourceHeader> files = fileHandler->getNetFileList();
    // tcp send
}

void Supervisor::handleCreate(ResourceHeader resourceHeader) {
    fileHandler->NewFileInfo(resourceHeader);
    // tcp duplicate ?
}

void Supervisor::handleDelete(ResourceHeader resourceHeader) {
    fileHandler->deleteFromNetList(resourceHeader);
    fileHandler->deleteNotOwnFile(resourceHeader);
}

void Supervisor::handleUpload(const Resource& res) {
    fileHandler->createFile(res);
}

void Supervisor::handleDownload(ResourceHeader resHeader) {
    Resource res = fileHandler->getFile(resHeader.name);
    if(strcmp(res.header.name, "") != 0) {
        sendUpload(res);
    }
}

int Supervisor::createFile(const std::string& path, const std::string& name) {
    ResourceHeader header = fileHandler->AddFile(path.c_str(), name.c_str(), "123");
    if(strcmp(header.name, "") != 0) {
        broadcastCreate(header);
        return 0;
    }
    return -1;
}

int Supervisor::downloadFile(const std::string& name) {
    ResourceHeader header = fileHandler->GetFileInfo(name.c_str());
    if(strcmp(header.name, "") != 0) {
        std::thread tcpServer(&NetworkHandler::runTCPClientThread, networkHandler, header);
        tcpServer.detach();
        return 0;
    }
    return -1;
}

int Supervisor::deleteFile(const std::string& name) {
    ResourceHeader header{};
    strcpy(header.name, name.c_str());
    int res = fileHandler->deleteOwnFile(header);
    if(res == 0) {
        broadcastDelete(header);
        return 0;
    }
    return -1;
}

std::vector<Resource> Supervisor::listDisk() {
    return fileHandler->getOwnFileList();
}

std::vector<ResourceHeader> Supervisor::listNetwork() {
    return fileHandler->getNetFileList();
}

void Supervisor::cleanUp() {
    shouldRun = false;
}
