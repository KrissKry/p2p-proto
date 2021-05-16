#include <iostream>

struct ResourceHeader
{
    char name[255];
    uint64_t size;
    char uuid[16];
    uint64_t timestamp;
};

struct Resource
{
    struct ResourceHeader;
    char *data;
};

struct User
{
    char uuid[128];
    char ip[16]; //: 192.168.0.x;
    unsigned int port;
};

struct ProtoPacket
{
    char command;
    struct ResourceHeader;
    char *data;
};

class NetworkComponent
{
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

class UI
{
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

class FileHandler
{
public:
private:
    int createFile();
    int deleteFile();
    int getFile();
};

int main(int argc, char *argv[])
{
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
