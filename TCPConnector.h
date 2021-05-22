#ifndef TIN_TCPHANDLER
#define TIN_TCPHANDLER


#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>

#include "Resource.h"

// enum connection_type {
//     SEND, RECEIVE
// };

class TCPConnector {


    public:
        TCPConnector(int socket, unsigned short port, unsigned long address) {

            remote_addr.sin_port = htonl(address);
            remote_addr.sin_addr.s_addr = htons(port);
            remote_addr.sin_family = AF_INET;
            memset(&(remote_addr.sin_zero), '\0', 8);

            this->socket = socket;
            this->port = port;
            this->address = address;
        }
        TCPConnector(const struct sockaddr_in &addr) {

            this->remote_addr = addr;
        }

        ~TCPConnector() {}



        int sendData(const struct Resource* file) {

            if( connect(socket, (struct sockaddr*)&remote_addr, sizeof(remote_addr)) < 0 ) {
                puts("[ERR] sendData() connect error\n");
                return -1;
            }

            if ( send(socket, file, sizeof(file), 0) < 0) {
                puts("[ERR] sendData() send error\n");
                return -1;
            }

            return 0;
        }


        int receiveData(struct Resource* file) {
            
            if( listen(socket, 1) < 0 ) {
                puts("[ERR] receiveData() listen error\n");
                return -1;
            }

            socklen_t len = sizeof(remote_addr);
            if ( accept(socket, (struct sockaddr *)&remote_addr, &len) < 0 ) {
                puts("[ERR] receiveData() accept error\n");
                return -1;
            }

            if (recv(socket, (void*)&file, sizeof(&file), 0) < 0) {
                puts("[ERR] sendData() receive error\n");
                return -1;
            }

            return 0;
        }
    
        
    private:
        int socket;
        unsigned short port;
        unsigned long address;
        sockaddr_in remote_addr;
};




#endif