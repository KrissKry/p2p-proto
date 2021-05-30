//
// Created by Michal Wiszenko on 30/05/2021.
//

#ifndef SUPERVISOR_H
#define SUPERVISOR_H

#include "Resource.h"

#define MAX_COMMAND_LENGTH 1024

class Supervisor {
private:
    std::mutex cmd_tx;
//    FileHandler fileHandler;
//    NetworkConnector networkConnector;

    void func();
public:
    Supervisor();

    void start();

    bool parseCommand(char *command);
};


#endif //SUPERVISOR_H
