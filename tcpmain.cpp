#include "TCPConnector.h"


#include <iostream>

int main(int argc, char *argv[]) {

    if (argc < 2) {
        puts("[ERR] Args missing");
        return -1;
    }



    sockaddr_in addr;

    int sock;
    if ( (sock = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        std::cout << "[ERR] " << strerror(errno);
        return -1;
    }
    // std::cout << "skarpeta " << sock << std::endl;


    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;
    memset( &addr.sin_zero, '\0', 8);


    if ( strcmp(argv[1], "SERVER") == 0) {

        Resource resource;
        strcpy(resource.header.name, "DROP DATABASE");
        strcpy( resource.header.uuid, "PC-1" );
        resource.header.timestamp = 12346277542; 

        std::string data = "aabbccddeeffggh";
        resource.data.assign(data.begin(), data.end());



        int bind_res, fd;
        if ( (bind_res = bind(sock, (struct sockaddr *)&addr, sizeof(addr))) < 0) {
            std::cout << "[ERR] " << strerror(errno);
            return -1;
        }

        TCPConnector connection(sock, addr);


        if (connection.serverListen() < 0) {
            std::cout << "[ERR] Exit in main @serverListen\n";
            return -1;
        }

        if ( (fd = connection.serverAccept() ) < 0 ) {
            std::cout << "[ERR] Exit in main @serverAccept\n";
            return -1;
        }

        /* send resource header */
        if ( connection.sendData(fd, static_cast<void *>(&resource.header), sizeof(resource.header)) < 0) {
            std::cout << "[ERR] Exit in main @header\n";
            return -1;
        }

        /* send resource data */
        if( connection.sendData(fd,  static_cast<void *>(&resource.data[0]), resource.data.size()) < 0) {
            std::cout << "[ERR] Exit in main @data\n";
            return -1;
        }





    } else if ( strcmp(argv[1], "CLIENT") == 0) {
        Resource dest;

        std::cout << "[I] Res size "<< sizeof(dest) <<"\n";
        std::cout << "[I] Header size " << sizeof(dest.header) << "\n";
        std::cout << "[I] Data size " << dest.header.size << "\n";
        const char* dest_address = "192.168.0.21";
        unsigned short dest_port = 8080;


        TCPConnector connection(sock, dest_port, dest_address);

        if ( connection.setupClient() < 0) {
            std::cout << "[ERR] Exit in main @setupClient\n";
            return -1;
        }

        /* receive header */
        if ( connection.receiveData( static_cast<void *>(&dest.header), sizeof(dest.header)) < 0) {
            std::cout << "[ERR] Exit in main @header\n";
            return -1;
        }

        /* receive resource data */
        dest.data.resize(dest.header.size);
        if ( connection.receiveData( static_cast<void *>(&dest.data[0]), dest.data.size()) < 0) {
            std::cout << "[ERR] Exit in main @data\n";
            return -1;
        }

        std::cout << "[I] Data received: " << &dest.data << "\n";
    } else {
        std::cout << "[ERR] Wrong command!\n";
    }



    // strcpy( resource.)
    


    return 0;
}