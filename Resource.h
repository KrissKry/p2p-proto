#include <stdint.h>

struct ResourceHeader
{
    char name[255];
    uint64_t size;
    char uuid[16];
    uint64_t timestamp;
};

struct Resource
{
    struct ResourceHeader header;
    char *data;
};