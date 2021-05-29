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
            if ( (receiver = accept(socket, (struct sockaddr *)&remote_addr, &len)) < 0 ) {
                puts("[ERR] sendData() accept error\n");
                return -1;
            }


            // int bytes_sent = 1;
            // int bytes_left_to_send = sizeof(file->header) + strlen(file->data);

            // while ( bytes_sent && bytes_left_to_send ) {

            //     bytes_sent = send( receiver, (void *) &file, bytes_left_to_send, 0);

            //     if (bytes_sent < 0 ) {
            //         std::cout << "[ERR] " << strerror(errno) <<"\n";
            //         return -1;
            //     }


            //     std::cout << "[I] Sent " << bytes_sent << " bytes\n";
            //     file += bytes_sent;
            //     bytes_left_to_send -= bytes_sent;                
            // }

            // std::cout << "[I] Sent everything\n";
            // int hdr
            if ( !sendHeader(socket, (void *)&file->header, sizeof(file->header)) ) {

            }

            if ( !sendData(socket, (void *)&file->data) ) {
                printError();
                return -1;
            }

            return 0;
        }


        int fetchData(struct Resource* file) {
            //zalozenie jest takie że przed pobraniem pliku, rezerwujemy przestrzen na 'dysku' na plik -> przekazana struktura jest juz w pelni gotowa na odebranie n bajtow
            std::cout << remote_addr.sin_addr.s_addr << " " << remote_addr.sin_port << std::endl;
            std::cout << "size of file:" << sizeof(file->header) << "\n";

            void *hdr = &file->header;
            void *rsr_data = &file->data;
            // std::cout<< &file->data << " " << rsr_data << "\n";
            int error;
            if( error = connect(socket, (struct sockaddr *)&remote_addr, sizeof(remote_addr)) < 0 ) {
                std::cout << "[ERR] " << strerror(errno) <<"\n";
                return -1;
            }


            // int bytes_recv = 1;
            // //p = static_cast<char*>(p) + 1;
            // if( !recv(socket, hdr, sizeof(file->header), 0) ) {
            //     std::cout << "[ERR] " << strerror(errno) <<"\n";
            //     return -1;
            // }



            // std::cout<<file->header.name << " xd \n";
            // while ( bytes_recv ) {

            //     bytes_recv = recv(socket, rsr_data, 1000, 0);

            //     std::cout << "[I] Recv " << bytes_recv << " bytes\n";

            //     rsr_data = static_cast<char*>(rsr_data) + bytes_recv;
            // }


            // // puts("[I] Received data :D");
            // std::cout<<"data "<<file->data<<"\n";


            if ( !recvHeader(socket, (void *)&file->header, sizeof(file->header)) ) {
                printError();
                return -1;
            }

            if ( !recvData(socket, (void *)&file->data)) {
                printError();
                return -1;
            }


            return 0;
        }
    
        
    private:
        int socket;
        unsigned short port;
        const char* address;
        sockaddr_in remote_addr;

        void printError() {
            std::cout << "[ERR] " << strerror(errno) << "\n";
        }

        int sendHeader(int sockfd, void* hdr, int size) {
            return send(sockfd, hdr, size, 0);
        }
        int recvHeader(int sockfd, void* hdr, int size) {
            return recv(sockfd, hdr, size, 0);
        }
        int sendData(int sockfd, void* data) {
            
            void* data_ptr = data;
            int bytes_sent = 0;
            int loop_send = 0;

            while ( true ) {
                
                if ( (loop_send = send(sockfd, data_ptr, 1000, 0)) < 0 ) {
                    printError();
                    return -1;
                }
                std::cout << "[I] Sent " << loop_send << " bytes\n";
                bytes_sent += loop_send;

                if ( loop_send == 0 ) {
                    // delete data_ptr;
                    return bytes_sent;
                }


                data_ptr = static_cast<char*>(data_ptr) + loop_send;
            }

        }
        int recvData(int sockfd, void* data) {

            void* data_ptr = data;
            int bytes_received = 0;
            int loop_receive = 0;

            while (true) {

                if ( (loop_receive = recv(sockfd, data_ptr, 1000, 0)) < 0 ) {
                    printError();
                    return -1;
                }
                std::cout << "[I] Received " << loop_receive << " bytes\n";

                bytes_received += loop_receive;

                if ( loop_receive == 0 )
                    return bytes_received;


                data_ptr = static_cast<char*>(data_ptr) + loop_receive;
            }
        }
};




#endif