//
// Created by Michal Wiszenko on 31.05.2021.
//

#include <iostream>
#include <vector>
#include "../include/UI.h"
#include "../include/utils.h"

UI::UI(Supervisor *supervisor): supervisor(supervisor) {

}

void UI::run() {

    while (parseCommand()) {
        printOptions();
    };

}

bool UI::parseCommand() {
    char command[MAX_COMMAND_LENGTH];
    safeInput(command);
    std::string str(command);
    std::vector<std::string> tokenList = splitStringOnSpace(str);
    int result;

    if (tokenList.size() == 1 && tokenList[0] == "quit") {
        return false;
    } else if (tokenList.size() == 4 && tokenList[0] == "upload" && tokenList[2] == "as") {
        result = supervisor->createFile(tokenList[1], tokenList[3]);
        if(result == 0) {
            safeOutput("OK");
        } else {
            safeOutput("Error");
        }
    } else if (tokenList.size() == 2 && tokenList[0] == "download") {
        result = supervisor->downloadFile(tokenList[1]);
        if(result == 0) {
            safeOutput("OK");
        } else {
            safeOutput("Error");
        }
    } else if (tokenList.size() == 2 && tokenList[0] == "delete") {
        result = supervisor->deleteFile(tokenList[1]);
        if(result == 0) {
            safeOutput("OK");
        } else {
            safeOutput("Error");
        }
    } else if (tokenList.size() == 2 && tokenList[0] == "list" && tokenList[1] == "disk") {
        std::vector<Resource> resourceList = supervisor->listDisk();

        if(resourceList.empty()) {
            safeOutput("No resources");
        } else {
            for (const Resource& res: resourceList) {
                std::cout<<res.header.name<<" "<<res.header.uuid<<std::endl;
            }
        }
    } else if (tokenList.size() == 2 && tokenList[0] == "list" && tokenList[1] == "net") {
        std::vector<std::pair<struct in_addr, ResourceHeader>> resourceList = supervisor->listNetwork();

        if(resourceList.empty()) {
            safeOutput("No resources");
        } else {
            for (std::pair<struct in_addr, ResourceHeader> res: resourceList) {
                std::cout<<res.second.name<<" "<<res.second.uuid<<std::endl;
            }
        }
    } else if (tokenList[0].empty()) {
        return true;
    } else {
        safeOutput("Invalid command");
    }
    return true;
}

void UI::safeOutput(const std::string &str) {
    std::lock_guard<std::mutex> lock(cmd_tx);
    std::cout << str << std::endl << std::flush;
}

char * UI::safeInput(char *command) {
    std::lock_guard<std::mutex> lock(cmd_tx);
    std::cin.getline(command, MAX_COMMAND_LENGTH);
    return command;
}


void UI::printOptions() {
    std::cout << "~~~~~~~~~~~~~~~~~\n";
    std::cout << "[I] ULICA WYSYŁA ZIOMECZKOM WIELKIE ELO\n";
    std::cout << "> upload <path> as <name>\n";
    std::cout << "> download <name>\n";
    std::cout << "> list disk\n";
    std::cout << "> list net\n";
    std::cout << "> quit\n";
    std::cout << "~~~~~~~~~~~~~~~~~\n";
}