//
// Created by Michal Wiszenko on 30/05/2021.
//

#include <iostream>
#include "../include/Supervisor.h"

Supervisor::Supervisor(): end(false) {
//    filehandler = FileHandler();
//    networkConnector = NetworkConnector();
    threadManager = new ThreadManager();
    network_handler = new NetworkHandler(tcp_upflow, tcp_downflow, udp_upflow, udp_downflow);
}

Supervisor::~Supervisor() { delete threadManager; }

void Supervisor::run() {
    // start network connector
    threadManager->createThread(std::thread(&Supervisor::listDisk, this));
    int i;
    while(!end) {
        {
            std::lock_guard<std::mutex> lock(incoming_tx);
            if (!incoming.empty()) {
                i = incoming.front();
                incoming.pop_front();
                std::cout<<i<<std::endl;
            }
        }
    }
}

void Supervisor::createFile(Resource res) {
//    fileHandler->createFile(res);
}

void Supervisor::deleteFile(ResourceHeader resHeader) {
//    fileHandler->deleteFile(resHeader);
}

int Supervisor::createFile(const std::string& path, const std::string& name) {
//    Resource* res = fileHandler->createFile(path, name);
//    if(res != nullptr) {
//        std::lock_guard<std::mutex> lock(outgoing_tx);
//        outgoing.push_back(1);
//        return 0;
//    }
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
    std::cout<<"a"<<std::endl;
    std::chrono::seconds dura( 5);
    std::this_thread::sleep_for( dura );
    //    return fileHandler->listDisk();
}

void Supervisor::cleanUp() {
    end = true;
    threadManager->cleanUp();
}