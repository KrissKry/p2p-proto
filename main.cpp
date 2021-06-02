#include "include/Supervisor.h"
#include "include/UI.h"

//void independentThread() {
//    safeOutput("Start thread");
//    std::this_thread::sleep_for(std::chrono::seconds(5));
//    safeOutput("Exit thread");
//}
//
//void threadCaller() {
//    std::thread t(independentThread);
//    t.detach();
//}

int runSend(SyncedDeque<char>& q) {
    char second = 'x';

    std::cout << "B4Send\n";

    q.push(second);

    std::cout << "AfterSend\n";
    return 0;
}

int runReceive(SyncedDeque<char>& q) {
    char first;

    std::cout << "B4Recv\n";
    q.pop(first);
    std::cout << "AfterRecv " << first;
    return 0;
}

int main(int argc, char *argv[]) {
    // Supervisor supervisor;
    // UI ui(&supervisor);
    // std::thread t1(&UI::run, std::ref(ui));
    // std::thread t2(&Supervisor::run, std::ref(supervisor));
    // t1.join();
    // supervisor.cleanUp();
    // t2.join();
    
    SyncedDeque<char> q;
    //jeśli funkcja oczekuje referencje na obiekt i uruchamiamy funkcję w nowym wątku to trzeba opakować ten obiekt w reference wrapper :DD
    std::thread t1(runReceive, std::ref(q));
    std::thread t2(runSend, std::ref(q));

    t1.join();
    t2.join();
    return 0;
}



