#include "../include/TCPConnector.h"


TCPConnector::TCPConnector(int socket, unsigned short port, const char* address) {
    
    addr.sin_port = htons(port);

    addr.sin_addr.s_addr = inet_addr(address);
    addr.sin_family = AF_INET;
    memset(&(addr.sin_zero), '\0', 8);

    // std::cout << addr.sin_port << " " << addr.sin_addr.s_addr << " " << address << std::endl;

    this->socket = socket;
    this->port = port;
    this->address = address;
}

TCPConnector::TCPConnector(int socket, const struct sockaddr_in &addr) {
    this->socket = socket;
    this->addr = addr;
}



int TCPConnector::setupClient() {
    int error;

    if ( (error = connect(this->socket, (struct sockaddr *)&addr, sizeof(addr))) < 0 ) {
        printError();
        return -1;
    }

    if (DEBUG_LOG) std::cout << "[DEBUG] TCP:: setupClient(), remote " << addr.sin_addr.s_addr << " on socket" << this->socket << "\n";

    return 0;
}



int TCPConnector::serverListen() {
    if( listen(socket, 10) < 0 ) {
        printError();
        return -1;
    }

    if (DEBUG_LOG) std::cout << "[DEBUG] TCP:: serverListen() on " << addr.sin_addr.s_addr << ":" << addr.sin_port << "\n";

    return 0;
}



int TCPConnector::serverAccept() {
    int receiver;
    socklen_t len = sizeof(addr);

    if ( (receiver = accept(socket, (struct sockaddr *)&addr, &len)) < 0 ) {
        printError();
        return -1;
    }

    if (DEBUG_LOG) std::cout << "[DEBUG] TCP:: serverAccept() connection on fd " << receiver << "\n";

    //zwrocenie deskryptora socketu do wysylania danych
    return receiver;
}



int TCPConnector::sendData(int sockfd, void *ptr, unsigned long long data_size) {
    int bytes_sent = 0;
    void* data_ptr = ptr;

    /*int error_code;
    unsigned int error_code_size = sizeof(error_code);
    int sockopt = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error_code, &error_code_size); */

    if (INFO_LOG) std::cout << "[I] TCP:: Beginning upload of " << data_size << " bytes.\n"; 
    bool resp;
    while( (bytes_sent = send(sockfd, data_ptr, data_size, 0) ) > 0) {

        resp = rg.transmissionFailure();
        std::cout << "[TCP]:: " << resp << "\n";
        if ( resp ) {
            std::cout << "[ERR] Simulated transmission error\n";
            return -1;
        }
        
        data_size -= bytes_sent;

        if (data_size < 1) {
            if (INFO_LOG) std::cout << "[I] TCP:: Sent all bytes. Exiting sendData()\n";
            return 0;
        }

        data_ptr = static_cast<char*>(data_ptr) + bytes_sent;
        
    }


    if (bytes_sent < 0) {
        printError();
        return -1;
    } else {
        return 0;
    }
}



int TCPConnector::receiveData(int fd, void *ptr, unsigned long long data_size) {

    int bytes_received = 0;
    void* data_ptr = ptr;
    int recv_fd;

    //receive data on correct descriptor = might be client's TCPConnector::socket or passed by server file descriptor
    if (fd != 0)
        recv_fd = fd;
    else
        recv_fd = this->socket;


    if (INFO_LOG) std::cout << "[I] TCP:: Beginning download of " << data_size << " bytes.\n"; 

    while( (bytes_received = recv(recv_fd, data_ptr, data_size, 0)) > 0) {


        data_size -= bytes_received;

        if (data_size < 1) {
            std::cout << "[I] Exiting receiveData()\n";

            return 0;
        }

        data_ptr = static_cast<char*>(data_ptr) + bytes_received;
    }


    if (bytes_received < 0 || data_size > 0) {
        printError();
        return -1;
    } else {
        return 0;
    }


}