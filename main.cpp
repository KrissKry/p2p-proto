#include "Supervisor.h"

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

//void independentThread() {
//    safeOutput("Start thread");
//    std::this_thread::sleep_for(std::chrono::seconds(5));
//    safeOutput("Exit thread");
//}
//
//void threadCaller() {
//    std::thread t(independentThread);
//    t.detach();
//}

int main(int argc, char *argv[]) {
    Supervisor supervisor;
    supervisor.start();
    return 0;
}
