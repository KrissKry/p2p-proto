//
// Created by Michal Wiszenko on 30/05/2021.
//

#ifndef SUPERVISOR_H
#define SUPERVISOR_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include <atomic>
#include "Constants.h"
#include "Resource.h"
#include "SyncedDeque.h"
#include "NetworkHandler.h"
#include "FileHandler.h"

class Supervisor
{
private:
    bool shouldRun = true;
    std::atomic<bool> stop = false;
    struct in_addr ip;

    FileHandler *fileHandler;
    NetworkHandler *networkHandler;

    SyncedDeque<std::pair<int, ProtoPacket>> tcp_downflow;
    SyncedDeque<std::pair<int, ProtoPacket>> tcp_upflow;
    SyncedDeque<ProtoPacket> udp_downflow;
    SyncedDeque<std::pair<struct in_addr, ProtoPacket>> udp_upflow;

    void tcpQueueListener();
    void udpQueueListener();

    void handleGetInfo();
    void handleCreate(ResourceHeader header, struct in_addr ip);
    void handleDelete(ResourceHeader resHeader, struct in_addr ip);
    void handleDownload(int fd, ResourceHeader resHeader);
    void handleUpload(const Resource &res);
    void handleNotFound(int fd, ResourceHeader resHeader);
    void broadcastCreate(ResourceHeader resourceHeader);
    void broadcastDelete(ResourceHeader resourceHeader);

    void sendDownload(ResourceHeader resourceHeader);
    void sendUpload(int fd, const Resource &res);
    void sendNotFound(int fd, const Resource &res);

public:
    Supervisor(struct in_addr);
    ~Supervisor();
    void run();
    void cleanUp();

    void broadcastGetInfo();
    int createFile(const std::string &path, const std::string &name);
    int downloadFile(const std::string &name);
    int deleteFile(const std::string &name);
    std::vector<Resource> listDisk();
    std::vector<std::pair<struct in_addr, ResourceHeader>> listNetwork();
};

#endif //SUPERVISOR_H
