//
// Created by Michal Wiszenko on 30.05.2021.
//

#ifndef THREADMANAGER_H
#define THREADMANAGER_H


#include <thread>
#include <vector>

class ThreadManager {
private:
    std::vector<std::thread> threads;

public:
    void createThread(std::thread);
    void cleanUp();
};


#endif //THREADMANAGER_H
