#ifndef TIN_TCPHANDLER
#define TIN_TCPHANDLER


#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>

#include "Resource.h"

// enum connection_type {
//     SEND, RECEIVE
// };

class TCPConnector {


    public:
        TCPConnector(int socket, unsigned short port, const char* address) {

            remote_addr.sin_port = htons(port);
            
            remote_addr.sin_addr.s_addr = inet_addr(address);
            remote_addr.sin_family = AF_INET;
            memset(&(remote_addr.sin_zero), '\0', 8);

            // std::cout << remote_addr.sin_port << " " << remote_addr.sin_addr.s_addr << " " << address << std::endl;

            this->socket = socket;
            this->port = port;
            this->address = address;
        }

        TCPConnector(int socket, const struct sockaddr_in &addr) {
            this->socket = socket;
            this->remote_addr = addr;
        }

        ~TCPConnector() {}


        int responseData(const struct Resource* file) {

            std::cout << "[I] Listening on " << remote_addr.sin_addr.s_addr << ":" << remote_addr.sin_port << std::endl;
            if( listen(socket, 1) < 0 ) {
                puts("[ERR] sendData() listen error\n");
                return -1;
            }

            socklen_t len = sizeof(remote_addr);
            int receiver;
            if ( receiver = accept(socket, (struct sockaddr *)&remote_addr, &len) < 0 ) {
                puts("[ERR] sendData() accept error\n");
                return -1;
            }


            int bytes_sent = 1;
            int bytes_left_to_send = sizeof(&file);
            while ( bytes_sent && bytes_left_to_send ) {

                bytes_sent = send( receiver, (void *) &file, bytes_left_to_send, 0);

                if (bytes_sent < 0 ) {
                    puts("[ERR] Sending file failed!");
                    return -1;
                }

                std::cout << "[I] Sent " << bytes_sent << " bytes\n";
                file += bytes_sent;
                bytes_left_to_send -= bytes_sent;
                
            }

            std::cout << "[I] Sent everything\n";
            return 0;
        }


        int fetchData(struct Resource* file) {
            //zalozenie jest takie że przed pobraniem pliku, rezerwujemy przestrzen na 'dysku' na plik -> przekazana struktura jest juz w pelni gotowa na odebranie n bajtow
            std::cout << remote_addr.sin_addr.s_addr << " " << remote_addr.sin_port << std::endl;
            int error;
            if( error = connect(socket, (struct sockaddr *)&remote_addr, sizeof(remote_addr)) < 0 ) {
                puts("[ERR] fetchData() connect error");
                std::cout << "[ERR] " << strerror(errno) <<"\n";
                return -1;
            }

            // if ( recv(socket, (void *)&file->header, sizeof(file->header), 0) < 0) {
            //     puts("[ERR] fetchData() receiving header");
            //     return -1;
            // }
            int bytes_recv = 1;
            int bytes_left_to_recv = sizeof(&file);

            while ( bytes_recv ) {

                bytes_recv = recv(socket, (void*)&file, bytes_left_to_recv, 0);

                std::cout << "[I] Recv " << bytes_recv << " bytes\n";

                file += bytes_recv;
                bytes_left_to_recv -= bytes_recv;
            }


            puts("[I] Received data :D");

            return 0;
        }
    
        
    private:
        int socket;
        unsigned short port;
        const char* address;
        sockaddr_in remote_addr;
};




#endif