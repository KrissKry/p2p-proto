#include "../include/NetworkHandler.h"


int NetworkHandler::runTCPServer(int server_index)
{
    auto server_ref = tcp_connections.at(server_index);
    int fd_sock = 0;

    while (tcp_server_running)
    {

        if ((fd_sock = server_ref.first->serverAccept()))
        {
            //new thread to handle incoming connection without blocking the rest of the program 
            auto ptr = std::make_shared<std::thread>(std::bind(&NetworkHandler::handleTCPServerThread, this, fd_sock));
            network_threads.push_back(std::move(ptr));
        }
    }

    for (auto &t : network_threads)
        t->join();

    network_threads.clear();
    return 0;
}


int NetworkHandler::handleTCPServerThread(int socket)
{

    ProtoPacket packet;
    packet.command = Commands::DOWNLOAD; //SEND

    if (tcp_connections.at(0).first->receiveData(socket, static_cast<void *>(&packet.header), sizeof(packet.header)) < 0)
        return -1; //zwroc error jakis cos


    if (INFO_LOG) std::cout << "[I] NH:: Received request for " << packet.header.name << " on socket " << socket << "\n";

    auto pair = std::make_pair(socket, packet);

    //send request for a file
    tcp_upflow.push(pair);


    //wait for response for our socket
    while (tcp_downflow.pop(pair) < 0) {}


    packet.data = pair.second.data;

    if (INFO_LOG) std::cout << "[I] NH:: Got response from SV " << pair.second.header.name << "\n";

    //send packet header
    if (tcp_connections.at(0).first->sendData(socket, static_cast<void *>(&pair.second.header), sizeof(pair.second.header)) < 0)
        return -1;

    if (INFO_LOG) std::cout << "[I] NH:: Beginning transmission of data from " << pair.second.header.name << "\n";
    
    //send packet data
    if (tcp_connections.at(0).first->sendData(socket, static_cast<void *>(&pair.second.data[0]), pair.second.data.size()) < 0)
        return -1;

    return 0;
}


void NetworkHandler::udpDownflowQueueListener(std::atomic_bool &stop)
{
    ProtoPacket message;
    memset(&message, 0, sizeof(ProtoPacket));
    while (!stop)
    {
        if (DEBUG_LOG) std::cout << "[DEBUG] NH:: popping udp_downflow\n";
        udp_downflow.pop(message);

        if (DEBUG_LOG) std::cout << "[DEBUG] NH:: calling udp broadcast\n";
        udpClient.broadcast(message);
    }
}


int NetworkHandler::runTCPClientThread(const ResourceHeader &header)
{
    ProtoPacket packet{};
    packet.command = Commands::UPLOAD; //RECEIVE
    packet.header = header;

    int client_socket;

    if ((client_socket = openNewSocket()) < 0)
    {
        std::cout << "[ERR] " << strerror(errno);
        return -1;
    }
    int client_index = spawnClient(header, client_socket);

    //send header
    packet.data.resize(packet.header.size);

    if ( tcp_connections.at(client_index).first->setupClient() < 0) {
        strerror(errno);
        return -1;
    }

    if (INFO_LOG) std::cout << "[I] NH:: Sending request for " << packet.header.name << " on socket " << client_socket << "\n";

    int resp {};
    if ( (resp = tcp_connections.at(client_index).first->sendData(client_socket, static_cast<void *>(&packet.header), sizeof(packet.header))) < 0)
        return -1;

    //tutaj czytanie 1byte komendy zwrotnej jesli NOT FOUND to handling błędu i usunięcie tego zasobu i blokada jego dalszego pobierania

    //receive header
    if (tcp_connections.at(client_index).first->receiveData(0, static_cast<void *>(&packet.header), sizeof(packet.header)) < 0) {
        std::cout << "[ERR] Confirming header for " << packet.header.name << " failed.\n";
        packet.data.clear();
        packet.data.resize(0);
        return -1;
    }
    

    if (INFO_LOG)
        std::cout << "[I] NH:: Received confirmation for " << packet.header.name << " on socket " << socket << "\n";


    //resize data accordingly once again
    packet.data.resize(packet.header.size);

    //receive data
    if (tcp_connections.at(client_index).first->receiveData(0, static_cast<void *>(&packet.data[0]), packet.data.size()) < 0) {

        std::cout << "[ERR] Receiving complete data for " << packet.header.name << " failed. Erasing memory and aborting.\n";
        packet.data.clear();
        packet.data.resize(0);
        return -1;
    }

    if (INFO_LOG)
        std::cout << "[I] NH:: Received " << packet.data.size() << " bytes of " << packet.header.name << " on socket " << socket << "\n";

    tcp_upflow.push(std::make_pair(client_socket, packet));


    return 0;
}


int NetworkHandler::createNewTCPServer()
{

    //create server socket
    int server_socket, bind_status;
    if ((server_socket = openNewSocket()) < 0)
    {
        std::cout << "[ERR] " << strerror(errno);
        return -1;
    }

    //sockaddr setup for any incoming transactions on :8080
    struct sockaddr_in in_addr;
    in_addr.sin_family = AF_INET;
    in_addr.sin_port = htons(8080);
    in_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&in_addr.sin_zero, '\0', 8);


    //bind socket with sockaddr
    int resp = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &resp, sizeof(resp)) < 0)
        std::cout << "setsockopt(SO_REUSEADDR) failed\n";


    if ((bind_status = bind(server_socket, (struct sockaddr *)&in_addr, sizeof(in_addr))) < 0)
    {
        std::cout << "[ERR] " << strerror(errno);
        return -1;
    }

    //open new connection
    auto server_ptr = std::shared_ptr<TCPConnector>(new TCPConnector(server_socket, in_addr));

    //store information on the connection in deque
    std::lock_guard<std::mutex> lock(deque_lock);
    tcp_connections.push_front(std::make_pair(std::move(server_ptr), server_socket));

    //listen to the sound of silence
    if (tcp_connections.at(0).first->serverListen() < 0)
        return -1;

    tcp_server_running = true;

    //return server connection index (first connection)
    return 0;
}


int NetworkHandler::closeAllSockets()
{

    if (DEBUG_LOG) std::cout << "[DEBUG] NH:: Closing all sockets\n";

    int sockets_closed = -1;
    for (auto &connection : tcp_connections)
    {
        close(connection.second);
        ++sockets_closed;
    }

    return sockets_closed;
}


int NetworkHandler::spawnClient(const ResourceHeader &header, int client_socket)
{
    unsigned short dest_port = 8080;

    // create new TCP Connector with destination ip in header (owner of required file)
    auto client_ptr = std::shared_ptr<TCPConnector>(new TCPConnector(client_socket, dest_port, header.uuid));

    // lock tcp_connections deque to return correct index
    std::lock_guard<std::mutex> lock(deque_lock);

    if (DEBUG_LOG) std::cout << "[DEBUG] NH:: Spawned new TCP Client with socket " << client_socket << " and remote " << header.uuid << "\n";


    tcp_connections.push_back(std::make_pair(std::move(client_ptr), client_socket));

    return tcp_connections.size() - 1;
}


int NetworkHandler::openNewSocket()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (DEBUG_LOG)
        std::cout << "[DEBUG] NH:: Opened socket " << sock << "\n";

    return sock;
}

