//
// Created by Michal Wiszenko on 31.05.2021.
//

#include <iostream>
#include <vector>
#include "../include/UI.h"
#include "../include/utils.h"

UI::UI(Supervisor *supervisor) : supervisor(supervisor)
{
}

void UI::run()
{
    printOptions();
    while (parseCommand()) {};
}

bool UI::parseCommand()
{
    char command[MAX_COMMAND_LENGTH];
    safeInput(command);
    std::string str(command);
    std::vector<std::string> tokenList = splitStringOnSpace(str);
    int result;

    if (tokenList.size() == 1 && tokenList[0] == "quit")
    {
        return false;
    }
    else if (tokenList.size() == 4 && tokenList[0] == "upload" && tokenList[2] == "as")
    {
        result = supervisor->createFile(tokenList[1], tokenList[3]);
        if (result == 0)
        {
            std::string output = "\n>>> Uploaded " + tokenList[3] + "\n";
            safeOutput(output);
        }
        else
        {
            safeOutput("\n>>> ERROR\n");
        }
    }
    else if (tokenList.size() == 2 && tokenList[0] == "download")
    {
        result = supervisor->downloadFile(tokenList[1]);
        if (result == 0)
        {
            std::string output = "\n>>> Finished downloading " + tokenList[1] + "\n";
            safeOutput(output);
        }
        else
        {
            safeOutput("\n>>> ERROR\n");
        }
    }
    else if (tokenList.size() == 2 && tokenList[0] == "delete")
    {
        result = supervisor->deleteFile(tokenList[1]);
        if (result == 0)
        {
            std::string output = "\n>>> Deleted " + tokenList[1] + "\n";
            safeOutput(output);
        }
        else
        {
            safeOutput("\n>>> ERROR\n");
        }
    }
    else if (tokenList.size() == 2 && tokenList[0] == "list" && tokenList[1] == "disk")
    {
        std::vector<Resource> resourceList = supervisor->listDisk();


        if (resourceList.empty())
        {
            safeOutput("\n>>> No resources locally\n");
        }
        else
        {
            safeOutput("\n_________FILES_________\n");
            for (const Resource &res : resourceList)
            {
                std::cout << res.header.name << " " << res.header.uuid << std::endl;
            }
            safeOutput("_______________________\n");
        }
    }
    else if (tokenList.size() == 2 && tokenList[0] == "list" && tokenList[1] == "net")
    {
        std::vector<std::pair<struct in_addr, ResourceHeader>> resourceList = supervisor->listNetwork();

        if (resourceList.empty())
        {
            safeOutput("\n>>> No resources in the network\n");
        }
        else
        {
            safeOutput("\n_________FILES_________\n");
            safeOutput("NAME | OWNER_IP | COPY_IP");
            for (std::pair<struct in_addr, ResourceHeader> res : resourceList)
            {
                std::cout << res.second.name << " " << res.second.uuid << " " << inet_ntoa(res.first) << std::endl;
            }
            safeOutput("_______________________\n");
        }
    }
    else if (tokenList.size() == 1 && tokenList[0] == "getinf")
    {
        supervisor->broadcastGetInfo();
        safeOutput("\nBroadcasted request for resources info\n");
    }
    else if (tokenList[0].empty())
    {
        return true;
    }
    else if (tokenList.size() == 1 && tokenList[0] == "help" ) 
    {
        printOptions();
    }
    else
    {
        safeOutput("\n>>> Invalid command\n");
    }
    return true;
}

void UI::safeOutput(const std::string &str)
{
    std::lock_guard<std::mutex> lock(cmd_tx);
    std::cout << str << std::endl
              << std::flush;
}

char *UI::safeInput(char *command)
{
    std::lock_guard<std::mutex> lock(cmd_tx);
    std::cin.getline(command, MAX_COMMAND_LENGTH);
    return command;
}

void UI::printOptions()
{
    std::cout << "________________KOMENDY________________\n";
    std::cout << "upload <path> as <name>\n";
    std::cout << "download <name>\n";
    std::cout << "delete <name>\n";
    std::cout << "list disk\n";
    std::cout << "list net\n";
    std::cout << "getinf\n";
    std::cout << "help\n";
    std::cout << "quit\n";
    std::cout << "_______________________________________\n";
}