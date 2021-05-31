//
// Created by Michal Wiszenko on 30.05.2021.
//

#include "ThreadManager.h"

void ThreadManager::createThread(std::thread thread) {
    threads.emplace_back(std::move(thread));
}

void ThreadManager::cleanUp() {
    for(auto & thread : threads) {
        thread.join();
    }
}