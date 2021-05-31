#include <iostream>
#include "UDPClient.h"
int main()
{

    UDPClient c;
    c.greet();
    const char *data = "data to send :)";
    c.broadcast(data);
    // c.serwer();
    return 0;
}