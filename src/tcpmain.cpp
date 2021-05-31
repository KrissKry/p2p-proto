// #include "../include/TCPConnector.h"


// #include <iostream>
// #include <unistd.h>

// int main(int argc, char *argv[]) {

//     if (argc < 2) {
//         puts("[ERR] Args missing");
//         return -1;
//     }



//     sockaddr_in addr;

//     int sock;
//     if ( (sock = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
//         std::cout << "[ERR] " << strerror(errno);
//         return -1;
//     }

//     addr.sin_family = AF_INET;
//     addr.sin_port = htons(8080);
//     addr.sin_addr.s_addr = INADDR_ANY;
//     memset( &addr.sin_zero, '\0', 8);


//     if ( strcmp(argv[1], "SERVER") == 0) {

//         Resource resource;
//         strcpy(resource.header.name, "DROP DATABASE");
//         strcpy( resource.header.uuid, "PC-1" );
//         resource.header.timestamp = 12346277542; 


//         std::string data = "aabbccddeeff\0";
//         resource.data.assign(data.begin(), data.end());
//         resource.header.size = resource.data.size();


//         int bind_res, fd;

//         if ( (bind_res = bind(sock, (struct sockaddr *)&addr, sizeof(addr))) < 0) {
//             std::cout << "[ERR] " << strerror(errno);
//             return -1;
//         }

//         TCPConnector connection(sock, addr);

//         /* start listening on socket */
//         if (connection.serverListen() < 0) {
//             std::cout << "[ERR] Exit in main @serverListen\n";
//             return -1;
//         }

//         /* accept incoming request */
//         if ( (fd = connection.serverAccept() ) < 0 ) {
//             std::cout << "[ERR] Exit in main @serverAccept\n";
//             return -1;
//         }

//         /* send resource header */
//         if ( connection.sendData(fd, static_cast<void *>(&resource.header), sizeof(resource.header)) < 0) {
//             std::cout << "[ERR] Exit in main @header\n";
//             return -1;
//         }

//         /* send resource data */
//         if( connection.sendData(fd,  static_cast<void *>(&resource.data[0]), resource.data.size()) < 0) {
//             std::cout << "[ERR] Exit in main @data\n";
//             return -1;
//         }


//         close(fd);


//     } else if ( strcmp(argv[1], "CLIENT") == 0) {

//         Resource dest;
//         const char* dest_address = "192.168.0.21";
//         unsigned short dest_port = 8080;


//         TCPConnector connection(sock, dest_port, dest_address);

//         if ( connection.setupClient() < 0) {
//             std::cout << "[ERR] Exit in main @setupClient\n";
//             return -1;
//         }

//         /* receive header */
//         if ( connection.receiveData( static_cast<void *>(&dest.header), sizeof(dest.header)) < 0) {
//             std::cout << "[ERR] Exit in main @header\n";
//             return -1;
//         }

//         /* receive resource data */
//         dest.data.resize(dest.header.size);
//         if ( connection.receiveData( static_cast<void *>(&dest.data[0]), dest.data.size()) < 0) {
//             std::cout << "[ERR] Exit in main @data\n";
//             return -1;
//         }

//         std::cout << "[I] Data received: ";
//         for(int i = 0; i < dest.data.size(); i++) {
//             std::cout << dest.data.at(i);
//         }
//         std::cout << "\n";

//     } else {
//         std::cout << "[ERR] Wrong command!\n";
//     }



//     close(sock);
//     return 0;
// }