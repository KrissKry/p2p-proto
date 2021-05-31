#include <deque>
#include <thread>
#include <mutex>
#include "Resource.h"

class NetworkHandler {


    public:

        NetworkHandler() {}
        ~NetworkHandler() {}

        //ustawienie parametrów serwera
        void setupTCP() {}
        void setupUDP() {}


        // nasluchiwanie na tcp oraz udp?
        void runHandler() {}
        
        //supervisor wywoluje to gdy chcemy cos pobrac
        void sendRequest(struct ProtoPacket proto_p) {}


        //spawn watku na okreslona akcje typu obsłużenie połączenia przychodzącego (TCP), albo nadanie komunikatu do sieci (UDP)
        void createThread() {
            
            std::thread new_thread(runThread);

            // po przejsciu przez instrukcje, lock_guard odblokuje automatycznie
            std::lock_guard<std::mutex> lock(deque_lock);

            network_threads.push_back(new_thread);

        }

        //obsluga watku w zaleznosci od tego co jest wymagane
        void runThread() {
            // Schemat działania TCP:
            //1. odczytanie komunikatu wejsciowego (np żądanie zasobu po tcp)
            //2. wyslanie żądania o zasób w górę do supervisora po unikatowym id
            //3. odczytanie komunikatu zwrotnego ze wskaźnikiem do zasobu (albo przez referencję chyba nawet lepiej)
            //4. przekazanie zasobu do tcp -> wysłanie nagłówka, a potem danych  
            //5. zamknięcie socketu po wysłaniu wszystkiego

            // Schemat działania UDP:
            // ?
        }

        
    private:
        std::deque<std::thread> network_threads;
        std::mutex deque_lock;

        int socket;
        // std::deque<

};


// struct NetworkRequest {
//     int command;
// }