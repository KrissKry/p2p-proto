#ifndef TIN_NETHANDLER
#define TIN_NETHANDLER

#include <deque>
#include <thread>
#include <mutex>
#include <functional>
#include <unistd.h>

#include "Resource.h"
#include "TCPConnector.h"
#include "UDPClient.h"
#include "SyncedDeque.h"
#include "Constants.h"

class NetworkHandler
{

public:
    NetworkHandler(
        SyncedDeque<std::pair<int, ProtoPacket>> &tcp_up,
        SyncedDeque<std::pair<int, ProtoPacket>> &tcp_down,
        SyncedDeque<std::pair<struct in_addr, ProtoPacket>> &udp_up,
        SyncedDeque<ProtoPacket> &udp_down)
        : tcp_upflow(tcp_up),
          tcp_downflow(tcp_down),
          udp_upflow(udp_up),
          udp_downflow(udp_down)
    {
    }
    ~NetworkHandler() {}

    /*
    *   Runs TCP Server -> Accepts all incoming connections
    *   Spawns a new thread and handles it on the side, without interrupting the program
    *   @return 0 on success end
    */
    int runTCPServer(int server_index);

    void udpServerRun(std::atomic_bool &stop) { udpClient.server(udp_upflow, stop); }

    /*
    *   Handles incoming TCP connection on a separate thread
    *   @return 0 on successful conversation with some client, -1 on error
    */
    int handleTCPServerThread(int socket);

    void udpDownflowQueueListener(std::atomic_bool &stop);

    /*
    *   Runs TCP client on user file download request 
    *   Calls openNewSocket(), spawnClient()
    *   @return 0 on success, -1 on error
    */
    int runTCPClientThread(const ResourceHeader &header);

    /*
    *   Spawns new TCP server -> Opens new socket, binds socket to INADDR_ANY,
    *   Creates a new TCP Connector and starts listening
    *   @return 0 on success, -1 on error
    */
    int createNewTCPServer();

    /*
    *   Iterates through all tcp_connections closing socket for each connection
    *   @return number of sockets closed
    */
    int closeAllSockets();

private:
    std::deque<std::shared_ptr<std::thread>> network_threads;
    std::mutex deque_lock;

    std::deque<std::pair<std::shared_ptr<TCPConnector>, int>> tcp_connections;

    UDPClient udpClient;

    SyncedDeque<std::pair<int, ProtoPacket>> &tcp_downflow;
    SyncedDeque<std::pair<int, ProtoPacket>> &tcp_upflow;
    SyncedDeque<ProtoPacket> &udp_downflow;
    SyncedDeque<std::pair<struct in_addr, ProtoPacket>> &udp_upflow;

    bool tcp_server_running = false;

    /*
    *   Creates a new TCP Connector with @param client_socket and destination address from @param header
    *   @return tcp_connections index of the connector, -1 on error
    */
    int spawnClient(const ResourceHeader &header, int client_socket);

    /*
    *   Opens new socket (*nix -> file descriptor)
    *   @return file descriptor id on success, -1 on error 
    */
    int openNewSocket();
};

#endif