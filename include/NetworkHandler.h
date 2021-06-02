#include <deque>
#include <thread>
#include <mutex>
#include <functional>

#include "Resource.h"
#include "TCPConnector.h"
#include "UDPClient.h"
#include "SyncedDeque.h"

class NetworkHandler
{

public:
    NetworkHandler(
        SyncedDeque<std::pair<int, ProtoPacket>> &tcp_up,
        SyncedDeque<std::pair<int, ProtoPacket>> &tcp_down,
        SyncedDeque<ProtoPacket> &udp_up,
        SyncedDeque<ProtoPacket> &udp_down) : tcp_upflow(tcp_up),
                                              tcp_downflow(tcp_down),
                                              udp_upflow(udp_up),
                                              udp_downflow(udp_down)
    {
    }
    ~NetworkHandler() {}

    //https://stackoverflow.com/questions/31372028/c-sockets-server-doesnt-accept-multiple-clients-linux
    //slaba skalowalnosc (na poziomie zupelnie przekraczajacym wymagania naszego protokolu)
    //https://stackoverflow.com/questions/41687505/what-effect-does-insert-have-on-references-to-deques
    //refki raz ustawione nie sa tracone po wrzuceniu czegos do kolejki

    int handleTCPServer(int server_index)
    {
        auto server_ref = tcp_connections.at(server_index);
        int fd_sock = 0;

        while (tcp_server_running)
        {

            if ((fd_sock = server_ref.first->serverAccept()))
            {

                //nowy watek do obsłużenia połączenia bez blokowania następnych połączeń
                // std::shared_ptr< std::thread > ptr (new spawn(fd_sock));
                // auto ptr = std::make_shared<std::thread>(new spawn(fd_sock));
                // auto ptr = std::make_shared<std::thread>(new std::thread(&NetworkHandler::runTcpThread, this, fd_sock));
                auto ptr = std::make_shared<std::thread>(std::bind(&NetworkHandler::runTCPServerThread, this, fd_sock));
                network_threads.push_back(std::move(ptr));
            }
        }

        for (auto &t : network_threads)
            t->join();

        network_threads.clear();
        return 0;
    }

    void udpServerRun()
    {
        udpClient.server(udp_upflow);
    }
    int runUdpThread()
    {
        return 0;
    }
    void createThread() {}

    //obsluga watku w zaleznosci od tego co jest wymagane
    int runTCPServerThread(int socket)
    {
        // Schemat działania TCP:
        //1. odczytanie komunikatu wejsciowego (np żądanie zasobu po tcp)
        //2. wyslanie żądania o zasób w górę do supervisora po unikatowym id
        //3. odczytanie komunikatu zwrotnego ze wskaźnikiem do zasobu (albo przez referencję chyba nawet lepiej)
        //4. przekazanie zasobu do tcp -> wysłanie nagłówka, a potem danych
        //5. zamknięcie socketu po wysłaniu wszystkiego

        ProtoPacket packet;
        auto pair = std::make_pair(socket, packet);

        // read resource request and command
        std::cout << "packet vector size: " << packet.data.size() << "\n";
        if (tcp_connections.at(0).first->receiveData(static_cast<void *>(&packet), sizeof(packet.header) + 1) < 0)
            return -1; //zwroc error jakis cos

        //send request for a file
        tcp_upflow.push(pair);

        //wait for response for our socket
        while (tcp_downflow.pop(pair) < 0)
        {
        }

        std::cout << "Got packet with resource name: " << packet.header.name << "\n";
        std::cout << "packet vector size: " << packet.data.size() << "\n";

        //send packet header
        if (tcp_connections.at(0).first->sendData(socket, static_cast<void *>(&packet.header), sizeof(packet.header)) < 0)
            return -1;

        //send packet data
        if (tcp_connections.at(0).first->sendData(socket, static_cast<void *>(&packet.data[0]), packet.data.size()) < 0)
            return -1;

        // close(socket);
        return 0;
    }

        int runTCPClientThread( ProtoPacket& packet ) {
            int client_socket;

            if ( (client_socket = openNewSocket()) < 0) {
                std::cout << "[ERR] " << strerror(errno);
                return -1;
            }
            int client_index = spawnClient(packet, client_socket);

            //send command and header
            if( tcp_connections.at(client_index).first->sendData(client_socket, static_cast<void *>(&packet), sizeof(packet.header) + 1) < 0)
                return -1;

            //receive header
            if (tcp_connections.at(client_index).first->receiveData(static_cast<void *>(&packet.header), sizeof(packet.header)) < 0 )
                return -1;

            //resize data accordingly
            packet.data.resize(packet.header.size);
            //receive data
            if ( tcp_connections.at(client_index).first->receiveData(static_cast<void *>(&packet.data[0]), packet.data.size()) < 0 )
                return -1;

            // tcp_upflow.push(std::make_pair(client_socket, packet)) 
            //niepotrzebne bo mamy pakiet przez refke przekazany i zakladamy ze resource jest przekazany do pakietu przez refke tez?

            return 0;
        }

    //only once at the beginning!!
    int createNewTCPServer() {

        //create server socket
        int server_socket, bind_status;
        if ( (server_socket = openNewSocket()) < 0 ) {
            std::cout << "[ERR] " << strerror(errno);
            return -1;
        }

        //sockaddr setup for any incoming transactions on :8080
        struct sockaddr_in in_addr;
        in_addr.sin_family = AF_INET;
        in_addr.sin_port = htons(8080);
        in_addr.sin_addr.s_addr = INADDR_ANY;
        memset( &in_addr.sin_zero, '\0', 8);


        //bind socket with sockaddr
        if ( (bind_status = bind(server_socket, (struct sockaddr *)&in_addr, sizeof(in_addr))) < 0) {
            std::cout << "[ERR] " << strerror(errno);
            return -1;
        }

        //open new connection
        auto server_ptr = std::shared_ptr<TCPConnector>(new TCPConnector(server_socket, in_addr));

        //store information on the connection in deque
        std::lock_guard<std::mutex> lock(deque_lock);
        tcp_connections.push_front( std::make_pair(std::move(server_ptr), server_socket) );

        //listen to the sound of silence
        if (tcp_connections.at(0).first->serverListen() < 0)
            return -1;

        tcp_server_running = true;

        //return server connection index
        return 0;
    }

private:
    std::deque<std::shared_ptr<std::thread>> network_threads;
    std::mutex deque_lock;

    std::deque<std::pair<std::shared_ptr<TCPConnector>, int>> tcp_connections;

    UDPClient udpClient;

    SyncedDeque<std::pair<int, ProtoPacket>> &tcp_downflow;
    SyncedDeque<std::pair<int, ProtoPacket>> &tcp_upflow;
    SyncedDeque<ProtoPacket> &udp_downflow;
    SyncedDeque<ProtoPacket> &udp_upflow;

    bool tcp_server_running = false;

    int spawnClient(ProtoPacket& packet, int client_socket) {
        unsigned short dest_port = 8080;

        auto client_ptr = std::shared_ptr<TCPConnector>(new TCPConnector(client_socket, dest_port, packet.header.uuid));

        std::lock_guard<std::mutex> lock(deque_lock);

        tcp_connections.push_back( std::make_pair(std::move(client_ptr), client_socket) );

        return tcp_connections.size() - 1;

    }

    int openNewSocket()
    {
        return socket(AF_INET, SOCK_STREAM, 0);
    }
};