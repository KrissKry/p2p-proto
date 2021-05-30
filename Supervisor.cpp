//
// Created by Michal Wiszenko on 30/05/2021.
//

#include <string>
#include <vector>
#include "utils.h"
#include "Supervisor.h"

Supervisor::Supervisor() {
//    filehandler = FileHandler();
//    networkConnector = NetworkConnector();
}

void Supervisor::func() {
    safeOutput("abc", cmd_tx);
}

void Supervisor::start() {
    safeOutput("Session started", cmd_tx);

    // start network connector

    char command[MAX_COMMAND_LENGTH];
    while (parseCommand(command));
}

bool Supervisor::parseCommand(char *command) {
    safeInput(command, MAX_COMMAND_LENGTH, cmd_tx);
    std::string str(command);
    std::vector<std::string> tokenList = splitStringOnSpace(str);

    if(tokenList.size() == 1 && tokenList[0] == "quit") {
        return false;
    } else if (tokenList.size() == 4 && tokenList[0] == "upload" && tokenList[2] == "as") {
//        Resource resource = FileHandler.create(tokenList[1], tokenList[3], ip?);
//        if(resource != null) {
//            NetworkConnector.upload(resource);
//        } else {
//            safeOutput("Error", cmd_tx);
//        }
    } else if (tokenList.size() == 2 && tokenList[0] == "download") {
//        Resource resource = FileHandler.create(tokenList[1], tokenList[3], ip?);
//        if(resource != null) {
//            NetworkConnector.download(resource);
//        } else {
//            safeOutput("Error", cmd_tx);
//        }
    } else if (tokenList.size() == 2 && tokenList[0] == "delete") {
//        Resource resource = FileHandler.remove(tokenList[1], ip?);
//        if(resource != null) {
//            NetworkConnector.remove(resource);
//        } else {
//            safeOutput("Error", cmd_tx);
//        }    } else if (tokenList.size() == 2 && tokenList[0] == "list" && tokenList[1] == "disk") {
//        std::vector<Resource> resources = FileHandler.listDisk();
//        for(Resource res: resources) {
//            safeOutput(res.resourceHeader.name, cmd_tx);
//        }
    } else {
        safeOutput("Invalid command", cmd_tx);
    }
    return true;
}