#ifndef TIN_SYNCDEQ
#define TIN_SYNCDEQ

#include <deque>
#include <mutex>
#include <condition_variable>

// dla TCP zalozenie ze ProtoPacket moze miec puste pole data czy jakoś tak ;-;
// dla TCP w górę T : std::pair<int, ProtoPacket >
// dla TCP w dół T: std::pair< int, ProtoPacket >
// dla UDP T : ProtoPacket

template <typename T>
class SyncedDeque
{

    //https://stackoverflow.com/questions/20516773/stdunique-lockstdmutex-or-stdlock-guardstdmutex
    //https://stackoverflow.com/questions/43019598/stdlock-guard-or-stdscoped-lock
public:
    // T SyncedDeque ()
    void push(const T &message)
    {
        std::unique_lock<std::mutex> lock(q_lock);
        q.push_back(message);
        lock.unlock();
        cv.notify_one();
    }

    void pop(T &message)
    {
        std::unique_lock<std::mutex> lock(q_lock);

        cv.wait(lock, [&]
                { return !q.empty(); });

        message = q.front();
        q.pop_front();
    }

private:
    std::deque<T> q;
    std::mutex q_lock;
    std::condition_variable cv;
};

#endif
