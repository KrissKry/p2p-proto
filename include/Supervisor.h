//
// Created by Michal Wiszenko on 30/05/2021.
//

#ifndef SUPERVISOR_H
#define SUPERVISOR_H

#include <mutex>
#include "Resource.h"
#include "FileHandler.h"
#include "SyncedDeque.h"
#include <deque>
#include <condition_variable>
#include <atomic>
#include "NetworkHandler.h"

class Supervisor
{
private:
    bool shouldRun = true;

    FileHandler *fileHandler;
    NetworkHandler *networkHandler;

    SyncedDeque<std::pair<int, ProtoPacket>> tcp_downflow;
    SyncedDeque<std::pair<int, ProtoPacket>> tcp_upflow;
    SyncedDeque<ProtoPacket> udp_downflow;
    SyncedDeque<std::pair<struct in_addr, ProtoPacket>> udp_upflow;

    void tcpQueueListener();
    void udpQueueListener();

    void handleGetInfo();
    void handleCreate(ResourceHeader header);
    void handleDelete(ResourceHeader resHeader);
    void handleDownload(ResourceHeader resHeader);
    void handleUpload(const Resource &res);
    void broadcastCreate(ResourceHeader resourceHeader);
    void broadcastDelete(ResourceHeader resourceHeader);
    void broadcastGetInfo(ResourceHeader resourceHeader);
    void sendDownload(ResourceHeader resourceHeader);
    void sendUpload(const Resource& res);

public:
    Supervisor();
    ~Supervisor();
    void run();
    void cleanUp();

    int createFile(const std::string &path, const std::string &name);
    int downloadFile(const std::string &name);
    int deleteFile(const std::string &name);
    std::vector<Resource> listDisk();
    std::vector<ResourceHeader> listNetwork();
};

#endif //SUPERVISOR_H
