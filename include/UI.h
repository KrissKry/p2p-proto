//
// Created by Michal Wiszenko on 31.05.2021.
//

#ifndef UI_H
#define UI_H

#include <mutex>
#include "Supervisor.h"

#define MAX_COMMAND_LENGTH 1024

class UI {
private:
    std::mutex cmd_tx;
    char * safeInput(char *command);
    void safeOutput(const std::string &str);
    bool parseCommand();
    Supervisor * supervisor;
    void printOptions();
public:
    void run();
    explicit UI(Supervisor *supervisor);
};


#endif //UI_H
