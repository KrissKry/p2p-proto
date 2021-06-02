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

class Supervisor {
private:
    bool shouldRun = true;
    FileHandler * fileHandler;
    NetworkHandler* network_handler;

    SyncedDeque< std::pair<int, ProtoPacket > > tcp_downflow;
    SyncedDeque< std::pair<int, ProtoPacket > > tcp_upflow;
    SyncedDeque< ProtoPacket > udp_downflow;
    SyncedDeque< ProtoPacket > udp_upflow;

    void tcpQueueListener();
    void udpQueueListener();

public:
    Supervisor();
    ~Supervisor();
    void run();
    void cleanUp();

    int createFile(const std::string &path, const std::string &name);
    int downloadFile(const std::string &name);
    int deleteFile(const std::string &name);
    std::vector<Resource> listDisk();
    void createFile(Resource res);
    void deleteFile(ResourceHeader resHeader);
};


#endif //SUPERVISOR_H
