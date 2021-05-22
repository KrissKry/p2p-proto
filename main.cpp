#include <iostream>
#include <thread>
#include <vector>

#ifdef __linux__
#include <mutex>
#elif __APPLE__
#endif

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

char *safeInput(char *command) {
    m.lock();
    std::cin.getline(command, MAX_COMMAND_LENGTH);
    m.unlock();
    return command;
}

void independentThread() {
    safeOutput("Start thread");
    std::this_thread::sleep_for(std::chrono::seconds(5));
    safeOutput("Exit thread");
}

void threadCaller() {
    std::thread t(independentThread);
    t.detach();
}

void splitStringOnSpace(const std::string &s, std::vector<std::string> &v) {
    std::string temp;
    for (char i : s) {
        if (i == ' ') {
            v.push_back(temp);
            temp = "";
        } else {
            temp.push_back(i);
        }
    }
    v.push_back(temp);
}

bool parseCommand(char *command) {
    safeInput(command);
    std::string str(command);
    std::vector<std::string> tokenList;
    splitStringOnSpace(str, tokenList);

    if(tokenList.size() == 1 && tokenList[0] == "quit") {
        return false;
    } else if (tokenList.size() == 4 && tokenList[0] == "upload" && tokenList[2] == "as") {
        // FileHandler.upload(tokenList[1], tokenList[3]);
    } else if (tokenList.size() == 2 && tokenList[0] == "download") {
        // FileHandler.download(tokenList[1]);
    } else if (tokenList.size() == 2 && tokenList[0] == "delete") {
        // FileHandler.delete(tokenList[1]);
    } else {
        safeOutput("Invalid command");
    }
    return true;
}

int main(int argc, char *argv[]) {
    safeOutput("Session started");

    // utp listening thread

    // tcp listening thread

    char command[MAX_COMMAND_LENGTH];

    while (parseCommand(command));

    return 0;
}
