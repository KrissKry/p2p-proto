#include "TCPConnector.h"


#include <iostream>

int main(int argc, char *argv[]) {

    //3232246428 == 192.168.42.156 PC windows LISTEN
    //3232235537 == 192.168.0.17 laptop mint RECEIVE


    if (argc < 2) {
        puts("[ERR] Args missing");
        return -1;
    }



    sockaddr_in addr;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    std::cout << "skarpeta " << sock << std::endl;


    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;
    memset( &addr.sin_zero, '\0', 8);

    // int len = sizeof(addr);
    // bind(sock, (struct sockaddr*)&addr, len);

    // std::cout << argv[1];
    if ( strcmp(argv[1], "RESPOND") == 0) {

        //pc 192.168.42.156
        Resource resource;
        strcpy(resource.header.name, "DROP DATABASE");
        resource.header.size = 16;
        strcpy( resource.header.uuid, "PC-1" );
        resource.header.timestamp = 12346277542; 
        resource.data = new char [resource.header.size];
        // resource.data = "aabbccddeeffggh";
        strcpy(resource.data, "aabbccddeeffggh");
        int bind_res = bind(sock, (struct sockaddr *)&addr, sizeof(addr));


        TCPConnector connection(sock, addr);
        connection.responseData(&resource);
        


    } else if ( strcmp(argv[1], "FETCH") == 0) {
        Resource dest;
        dest.data = new char[16];

        // unsigned long dest_address = 3232246428;
        const char* dest_address = "192.168.42.156";
        unsigned short dest_port = 8080;

        TCPConnector connection(sock, dest_port, dest_address);
        std::cout << "fetching\n";
        connection.fetchData(&dest);
        std::cout << "post fetch\n";

        std::cout << dest.data << std::endl;
    } else {
        std::cout << "Wrong command!\n";
    }



    // strcpy( resource.)
    


    return 0;
}