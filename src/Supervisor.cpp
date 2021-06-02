//
// Created by Michal Wiszenko on 30/05/2021.
//

#include <iostream>
#include <thread>
#include "../include/Supervisor.h"

Supervisor::Supervisor(): end(false) {
    filehandler = FileHandler();
    network_handler = new NetworkHandler(tcp_upflow, tcp_downflow, udp_upflow, udp_downflow);
}

Supervisor::~Supervisor() { delete fileHandler; }

void Supervisor::run() {
    // start network connector
    std::thread tcpQueueListener(&Supervisor::tcpQueueListener, this);
    std::thread udpQueueListener(&Supervisor::udpQueueListener, this);

    tcpQueueListener.join();
    udpQueueListener.join();
}

void Supervisor::tcpQueueListener() {
    std::pair<int, ResourceHeader> message;
    while(shouldRun) {
        tcp_upflow.pop(message);
        std::cout<<message.first<<std::endl;
    }
}

void Supervisor::udpQueueListener() {
    std::pair<char, ResourceHeader> message;
    while(shouldRun) {
        udp_upflow.pop(message);
        std::cout<<message.first<<std::endl;
    }
}

void Supervisor::createFile(Resource res) {
//    fileHandler->createFile(res);
}

void Supervisor::deleteFile(ResourceHeader resHeader) {
//    fileHandler->deleteFile(resHeader);
}

int Supervisor::createFile(const std::string& path, const std::string& name) {
    int i = fileHandler->AddFile(path.c_str(), name.c_str(), "123");
    if(i == 0) {
        ResourceHeader resourceHeader{};
        const std::pair pair(1, resourceHeader);
        udp_downflow.push(pair);
        return 0;
    }
    return -1;
}

int Supervisor::downloadFile(const std::string& name) {
//    Resource* res = fileHandler->getFile(name);
//    if(res != nullptr) {
//        std::lock_guard<std::mutex> lock(outgoing_tx);
//        outgoing.push_back(2);
//        return 0;
//    }
    return -1;
}

int Supervisor::deleteFile(const std::string& name) {
//    Resource* res = fileHandler->deleteFile(name);
//    if(res != nullptr) {
//        std::lock_guard<std::mutex> lock(outgoing_tx);
//        outgoing.push_back(3);
//        return 0;
//    }
    return -1;
}

std::vector<Resource> Supervisor::listDisk() {
    return fileHandler->getOwnFileList();
}

void Supervisor::cleanUp() {
    shouldRun = false;
}
