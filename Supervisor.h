//
// Created by Michal Wiszenko on 30/05/2021.
//

#ifndef SUPERVISOR_H
#define SUPERVISOR_H

#include <mutex>
#include "Resource.h"
#include "ThreadManager.h"
#include <deque>
#include <condition_variable>
#include <atomic>

class Supervisor {
private:
    std::atomic<bool> end;

public:
    Supervisor();
    void run();
    void cleanUp();

    std::deque<int> incoming;
    std::deque<int> outgoing;
    std::mutex incoming_tx;
    std::mutex outgoing_tx;
//    FileHandler fileHandler;
//    NetworkConnector networkConnector;

    int createFile(const std::string &path, const std::string &name);
    int downloadFile(const std::string &name);
    int deleteFile(const std::string &name);
    std::vector<Resource> listDisk();
    void createFile(Resource res);
    void deleteFile(ResourceHeader resHeader);
};


#endif //SUPERVISOR_H
