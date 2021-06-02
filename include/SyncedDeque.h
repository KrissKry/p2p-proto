#ifndef TIN_SYNCDEQ
#define TIN_SYNCDEQ


#include <deque>
#include <mutex>
#include <condition_variable>
#include <type_traits>
// dla TCP zalozenie ze ProtoPacket moze miec puste pole data czy jakoś tak ;-;
// dla TCP w górę T : std::pair<int, ProtoPacket > 
// dla TCP w dół T: std::pair< int, ProtoPacket > 
// dla UDP T : ProtoPacket

template <typename T>
class SyncedDeque {

        //https://stackoverflow.com/questions/20516773/stdunique-lockstdmutex-or-stdlock-guardstdmutex
        //https://stackoverflow.com/questions/43019598/stdlock-guard-or-stdscoped-lock
    public:
        // T SyncedDeque ()
        void push(const T& message) {
            std::unique_lock<std::mutex> lock(q_lock);
            q.push_back(message);
            lock.unlock();
            cv.notify_one();
        }


        int pop(T& message) {
            std::unique_lock<std::mutex> lock(q_lock);

            cv.wait(lock,[&] { return !q.empty();});

            //czy typ wiadomosci jest odpowiedni dla tcp 
            if constexpr (std::is_same_v<T, std::pair<int, ProtoPacket> >) {

                //jesli jest to sprawdzic czy komunikat jest dla naszego scoketu
                if (q.front().first == message.first) {

                    //jesli tak to kopiujemy i wychodzimy
                    message = q.front();
                    q.pop_front();
                    return 0;
                } else {
                    
                    //pozwalamy innemu wątkowi zgarnąć swoją wiadomość
                    cv.notify_one();
                    return -1;
                }

                
            } else {
                message = q.front();
                q.pop_front();
                return 0;
            }
        }
            // auto it = std::find_if(q.begin(), q.end(), [socket] (const auto& pair) { return pair.first == socket; } );

    private:

        std::deque<T> q;
        std::mutex q_lock;
        std::condition_variable cv;

};



#endif