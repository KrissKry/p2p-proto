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

int main(int argc, char *argv[]) {
    Supervisor supervisor;
    UI ui(&supervisor);
    std::thread t1(&UI::run, std::ref(ui));
    std::thread t2(&Supervisor::run, std::ref(supervisor));
    t1.join();
    supervisor.cleanUp();
    t2.join();
    return 0;
}
