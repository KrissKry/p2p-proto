#ifndef TIN_RESOURCE
#define TIN_RESOURCE

#include <stdint.h>
#include <vector>

namespace Commands
{
    const char CREATE = '1';
    const char DELETE = '2';
    const char DOWNLOAD = '3';
    const char UPLOAD = '4';
    const char NOT_FOUND = '5';
    const char GET_INFO = '6';
    const char SEND_INFO = '7';
}

// #pragma pack(push, 1)
struct ResourceHeader
{
    char name[255];
    uint64_t size;
    char uuid[16];
    uint64_t timestamp;
};

// #pragma pack(pop)
struct Resource
{
    struct ResourceHeader header;
    // char *data;
    std::vector<unsigned char> data;
};

// #pragma pack(push, 1)
struct ProtoPacket
{
    char command;
    struct ResourceHeader header;
    std::vector<unsigned char> data;
};
// #pragma pack(pop)
#endif
