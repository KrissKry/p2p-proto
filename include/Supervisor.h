//
// Created by Michal Wiszenko on 30/05/2021.
//

#ifndef SUPERVISOR_H
#define SUPERVISOR_H

#include <mutex>
#include "Resource.h"
#include "FileHandler.h"
#include "SyncedDeque.h"
//#include "NetworkHandler.h"
#include <deque>
#include <condition_variable>
#include <atomic>

class Supervisor {
private:
    bool shouldRun = true;
    FileHandler * fileHandler;
//    NetworkHandler * networkHandler;

public:
    Supervisor();
    ~Supervisor();
    void run();
    void cleanUp();

    SyncedDeque<std::pair<int, ResourceHeader>> incomingTcp;
    SyncedDeque<std::pair<int, Resource>> outgoingTcp;
    SyncedDeque<std::pair<char, ResourceHeader>> incomingUdp;
    SyncedDeque<std::pair<char, ResourceHeader>> outgoingUdp;

    int createFile(const std::string &path, const std::string &name);
    int downloadFile(const std::string &name);
    int deleteFile(const std::string &name);
    std::vector<Resource> listDisk();
    void createFile(Resource res);
    void deleteFile(ResourceHeader resHeader);

    void tcpQueueListener();

    void udpQueueListener();
};


#endif //SUPERVISOR_H
