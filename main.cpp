#include <iostream>
#include <thread>
#include <cstdlib>
#include <vector>
#include <unistd.h>

#define MAX_COMMAND_LENGTH 1024

struct ResourceHeader {
    char name[255];
    uint64_t size;
    char uuid[16];
    uint64_t timestamp;
};

struct Resource {
    struct ResourceHeader;
    char *data;
};

struct User {
    char uuid[128];
    char ip[16]; //: 192.168.0.x;
    unsigned int port;
};

struct ProtoPacket {
    char command;
    struct ResourceHeader;
    char *data;
};

class NetworkComponent {
public:
    NetworkComponent();

    ~NetworkComponent();

    // TCP
    // int sendFile(const struct& Resource res);
    // int receiveFile();
    // UDP - napisze ja (debil znajde cię)
    // int broadcast(const struct& ProtoPacket pack)

private:
};

class UI {
public:
    UI();

    ~UI();

private:
    void listOptions();
    // void
    // int createFile();
    // int deleteFile();
    // int getFile();
    // int sendFile();
    // int broadcast();
    // void listDisk();
};

class FileHandler {
public:
private:
    int createFile();

    int deleteFile();

    int getFile();
};

std::mutex m;

void safeOutput(const std::string &str) {
    m.lock();
    std::cout << str << std::endl << std::flush;
    m.unlock();
}

char* safeInput(char* command) {
    m.lock();
    std::cin.getline(command, MAX_COMMAND_LENGTH);
    m.unlock();
    return command;
}

void splitCommandOnSpace(const std::string& s, std::vector<std::string>& v) {
    std::string temp;
    for(char i : s){
        if(i==' '){
            v.push_back(temp);
            temp = "";
        }
        else{
            temp.push_back(i);
        }
    }
    v.push_back(temp);
}

int parseCommand(char *command) {
    std::string str(command);
    std::vector<std::string> tokenList;
    splitCommandOnSpace(str, tokenList);

    if (tokenList[0] == "quit") {
        return 0;
    } else if (tokenList[0] == "upload") {
        return 1;
    }
    return -1;
}

void independentThread()
{
    safeOutput("Start thread");
    std::this_thread::sleep_for(std::chrono::seconds(5));
    safeOutput("Exit thread");
}

void threadCaller()
{
    std::thread t(independentThread);
    t.detach();
}

int main(int argc, char *argv[]) {
    safeOutput("Session started");

    // utp listening thread

    // tcp listening thread

    char command[MAX_COMMAND_LENGTH];
    int parseResult;

    while (safeInput(command)) {
        parseResult = parseCommand(command);
        switch (parseResult) {
            case -1:
                safeOutput("Invalid command");
                break;
            case 0:
                exit(0);
            case 1:
                threadCaller();
                break;
            default:
                break;
        }
    }

    /* podział prac
    Waldek - UDP
    Konrad - FileHandler - stworzenie zasobu, usunięcie go, przechowywanie w koneterze, zwrócenie wszystkich,
    Michał - main.cpp / fredy - spawnowanie
    Ja :D  - TCP send/recv 

    todo cmake/make na ./include ./src
    deadline 22.05
    pisanie funkcji, tak, żeby dało się je wykorzystywać na wyższym poziomie (parent-module) essa
    */

    return 0;
}
