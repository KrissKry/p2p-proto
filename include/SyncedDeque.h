#ifndef TIN_SYNCDEQ
#define TIN_SYNCDEQ

#include <iostream>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <type_traits>
#include <atomic>
#include "Resource.h"
#include "Constants.h"

template <typename T>
class SyncedDeque
{
    //https://stackoverflow.com/questions/20516773/stdunique-lockstdmutex-or-stdlock-guardstdmutex
    //https://stackoverflow.com/questions/43019598/stdlock-guard-or-stdscoped-lock
public:

    /* sets stopper variable to exit prematurely from pop lock */
    void setStopper( std::atomic_bool &stop) {
        // std::cout << "Q:: "<< &stop << " " << stop << "\n";  
        stopper = &stop; 
        // std::cout << "Q::stopper " << stopper << " " << *stopper << "\n";
    }

    /* push message to shared deque */
    void push(const T &message) {
        std::unique_lock<std::mutex> lock(q_lock);
        q.push_back(message);

        if (DEBUG_LOG) std::cout << "[DEBUG] Q:: pushing message: " << typeid(message).name() << "\n";
        
        if constexpr (std::is_same_v<T, std::pair<struct in_addr, ProtoPacket> >)
        {
            if (DEBUG_LOG) std::cout << "[DEBUG] Q:: udp que push: " << q.front().second.command << std::endl;
        }
        lock.unlock();
        cv.notify_one();
    }

    /* pop message from shared deque if there is any */
    int pop(T &message) {   

        std::unique_lock<std::mutex> lock(q_lock);

        cv.wait(lock, [this]()
                { 
                    // std::cout << "[WEEoo] " << typeid(T).name() << " " << stopper<< " " << ( 0 == *stopper ? "true" : "false")  << "\n";
                    return ((!q.empty()) || *stopper); });

        if(*stopper ) {
            // std::cout << "[WEEEOOOWEOOO]" << *stopper << "\n";
            return -1;
        } else {
            // std::cout << "[weqq] Stopper is false " << stopper << " " << *stopper << "\n";
        }

        //czy typ wiadomosci jest odpowiedni dla tcp
        if constexpr (std::is_same_v<T, std::pair<int, ProtoPacket>>)
        {

            //jesli jest to sprawdzic czy komunikat jest dla naszego scoketu
            if (q.front().first == message.first || message.first == -1)
            {
                if (DEBUG_LOG) std::cout << "[DEBUG] Q:: Pop with message for socket " << message.first << "\n";
                if (q.size() > 0)
                    if (DEBUG_LOG) std::cout << "[DEBUG] Q:: Pop with q front message for socket " << q.front().first << "\n";

                //jesli tak to kopiujemy i wychodzimy
                message = q.front();
                q.pop_front();
                return 0;
            }
            else
            {
                // std::cout << "POP ELSE ;_;\n";
                if (DEBUG_LOG) std::cout << "[DEBUG] Q:: Pop: " << message.first << "\n";
                if (q.size() > 0)
                    std::cout << "Q: " << q.front().first << "\n";
                //pozwalamy innemu wątkowi zgarnąć swoją wiadomość
                cv.notify_one(); 
                return -1;
            }
        }
        else
        {
            message = q.front();
            q.pop_front();
            return 0;
        }
    }
    bool triggerUpdate() {
        cv.notify_all();
        return true;
    }

private:
    std::atomic_bool* stopper;
    std::deque<T> q;
    std::mutex q_lock;
    std::condition_variable cv;
};

#endif
