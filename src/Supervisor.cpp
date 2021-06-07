//
// Created by Michal Wiszenko on 30/05/2021.
//

#include <iostream>
#include <thread>
#include "../include/Supervisor.h"

Supervisor::Supervisor(struct in_addr ip) : ip(ip)
{
    fileHandler = new FileHandler();
    tcp_downflow.setStopper(this->stop);
    tcp_upflow.setStopper(this->stop);
    udp_downflow.setStopper(this->stop);
    udp_upflow.setStopper(this->stop);
    networkHandler = new NetworkHandler(tcp_upflow, tcp_downflow, udp_upflow, udp_downflow);
}

Supervisor::~Supervisor()
{
    delete fileHandler;
    delete networkHandler;
}

void Supervisor::run()
{
    // start network connector
    std::thread tcpQueueListener(&Supervisor::tcpQueueListener, this);
    std::thread udpQueueListener(&Supervisor::udpQueueListener, this);

    int res = networkHandler->createNewTCPServer();
    if (res == 0)
    {
        std::thread tcpServer(&NetworkHandler::handleTCPServer, networkHandler, 0);
        tcpServer.detach();
    }

    std::thread udpServer(&NetworkHandler::udpServerRun, networkHandler);
    udpServer.detach();
    std::thread udpDownflowQueListener(&NetworkHandler::udpDownflowQueueListener, networkHandler);
    udpDownflowQueListener.detach();
    if (DEBUG_LOG) std::cout << "[DEBUG] SV:: udpDownlow detach\n";
    tcpQueueListener.join();
    udpQueueListener.join();
}

void Supervisor::tcpQueueListener()
{
    std::pair<int, ProtoPacket> message;
    
    while (shouldRun)
    {   
        message.first = -1;
        tcp_upflow.pop(message);
        if (DEBUG_LOG) std::cout << "[DEBUG] SV:: got command " << message.second.command << " on socket " << socket << "\n";

        Resource res = {};
        res.header = message.second.header;
        res.data = message.second.data;
        // std::cout << res.header.name << " x x x " << res.header.size << "\n";
        switch (message.second.command)
        {
        case Commands::DOWNLOAD:
            handleDownload(message.first, res.header);
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
        if (DEBUG_LOG) std::cout << "[DEBUG] SV:: udpQueueListener: waiting for message" << "\n";
        udp_upflow.pop(message);

        switch (message.second.command)
        {
        case Commands::CREATE:
            if (DEBUG_LOG) std::cout << "[DEBUG] SV:: udpQueueListener:switch create" << "\n";
            handleCreate(message.second.header, message.first);
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

void Supervisor::broadcastCreate(ResourceHeader resourceHeader)
{
    ProtoPacket protoPacket;
    protoPacket.command = Commands::CREATE;
    protoPacket.header = resourceHeader;
    if (INFO_LOG) std::cout << "[I] SV:: broadcastCreate for " << resourceHeader.name << "\n";

    udp_downflow.push(protoPacket);
}

void Supervisor::broadcastDelete(ResourceHeader resourceHeader)
{
    ProtoPacket protoPacket;
    protoPacket.command = Commands::DELETE;
    protoPacket.header = resourceHeader;

    udp_downflow.push(protoPacket);
}

void Supervisor::broadcastGetInfo(ResourceHeader resourceHeader)
{
    ProtoPacket protoPacket;
    protoPacket.command = Commands::GET_INFO;
    protoPacket.header = resourceHeader;
    udp_downflow.push(protoPacket);
}

void Supervisor::sendDownload(ResourceHeader resourceHeader)
{
    ProtoPacket protoPacket;
    protoPacket.command = Commands::DOWNLOAD;
    protoPacket.header = resourceHeader;
    const std::pair message(1, protoPacket);
    tcp_downflow.push(message);
}

void Supervisor::sendUpload(int fd, const Resource &res)
{
    ProtoPacket protoPacket;
    protoPacket.command = Commands::UPLOAD;
    protoPacket.header = res.header;
    protoPacket.data = res.data;
    const std::pair message(fd, protoPacket);
    tcp_downflow.push(message);
}

void Supervisor::handleGetInfo()
{
    std::vector<Resource> files = fileHandler->getOwnFileList();
    for (const Resource& res: files) {
        broadcastCreate(res.header);
    }
}

void Supervisor::handleCreate(ResourceHeader resourceHeader, struct in_addr senderIp)
{
    fileHandler->NewFileInfo(resourceHeader, senderIp);
}

void Supervisor::handleDelete(ResourceHeader resourceHeader)
{
    fileHandler->deleteFromNetList(resourceHeader);
    fileHandler->deleteNotOwnFile(resourceHeader);
}

void Supervisor::handleUpload(const Resource &res)
{
    int result = fileHandler->createFile(res);
    if (result == 0) {
        broadcastCreate(res.header);
    }
}

void Supervisor::handleDownload(int fd, ResourceHeader resHeader)
{
    if (DEBUG_LOG) std::cout << "[DEBUG] SV:: Handling download request for " << resHeader.name << "\n";// << resHeader.uuid << "\n";
    Resource res = fileHandler->getFile(resHeader.name);
    // std::cout << res.header.name << " " << res.header.size << " " << res.header.uuid << "\n";
    if (strcmp(res.header.name, "") != 0)
    {
        if (DEBUG_LOG) std::cout << "[DEBUG] SV:: Res.header not empty\n";
        sendUpload(fd, res);
    }
}

int Supervisor::createFile(const std::string &path, const std::string &name)
{
    ResourceHeader header = fileHandler->AddFile(path.c_str(), name.c_str(), ip);
    if (strcmp(header.name, "") != 0)
    {
        if (DEBUG_LOG) std::cout << "[DEBUG] SV:: createFile\n";
        broadcastCreate(header);
        return 0;
    }
    return -1;
}

int Supervisor::downloadFile(const std::string &name)
{
    ResourceHeader header = fileHandler->GetFileInfo(name.c_str());
    if (strcmp(header.name, "") != 0)
    {
        std::thread tcpServer(&NetworkHandler::runTCPClientThread, networkHandler, header);
        tcpServer.detach();
        return 0;
    }
    return -1;
}

int Supervisor::deleteFile(const std::string &name)
{
    Resource res = fileHandler->getFile(name.c_str());
    ResourceHeader header = res.header;
    if (strcmp(header.name, "") != 0)
    {
        if(strcmp(header.uuid, inet_ntoa(ip)) == 0) {
            fileHandler->deleteOwnFile(header);
            broadcastDelete(header);
        } else {
            fileHandler->deleteNotOwnFile(header);
        }
        return 0;
    }
    return -1;
}

std::vector<Resource> Supervisor::listDisk()
{
    return fileHandler->getOwnFileList();
}

std::vector<std::pair<struct in_addr, ResourceHeader>> Supervisor::listNetwork()
{
    return fileHandler->getNetFileList();
}

void Supervisor::cleanUp()
{
    shouldRun = false;
    stop = true;
}
