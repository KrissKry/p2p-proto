#ifndef RESOURCE_H
#define RESOURCE_H

#include <stdint.h>
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
    struct ResourceHeader resourceHeader;
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
    struct ResourceHeader resourceHeader;
    char *data;
};
#endif