<<<<<<< HEAD
#include <stdint.h>
#include <vector>

=======
#ifndef RESOURCE_H
#define RESOURCE_H

#include <stdint.h>
>>>>>>> threads
namespace Commands
{
    const char CREATE = '1';
    const char DELETE = '2';
    const char DOWNLOAD = '3';
    const char UPLOAD = '4';
    const char NOT_FOUND = '5';
    const char GET_INFO = '6';
    const char SEND_INFO = '7';
    const char DUPLICATE = '8';

}

struct ResourceHeader
{
    char name[255];
    uint64_t size;
    char uuid[16];
    uint64_t timestamp;
};

struct Resource
{
<<<<<<< HEAD
    struct ResourceHeader header;
    // char *data;
    std::vector<unsigned char> data;
=======
    struct ResourceHeader resourceHeader;
    char *data;
};

struct User
{
    char uuid[128];
    char ip[16]; //: 192.168.0.x;
    unsigned int port;
>>>>>>> threads
};

struct ProtoPacket
{
    char command;
    struct ResourceHeader resourceHeader;
<<<<<<< HEAD
    std::vector<unsigned char> data;
};
=======
    char *data;
};
#endif
>>>>>>> threads
