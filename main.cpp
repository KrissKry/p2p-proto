#include <thread>
#include "include/Supervisor.h"
#include "include/UI.h"

int main(int argc, char *argv[]) {
    Supervisor supervisor;
    UI ui(&supervisor);
    std::thread uiRun(&UI::run, std::ref(ui));
    std::thread supervisorRun(&Supervisor::run, std::ref(supervisor));
    uiRun.join();
    supervisor.cleanUp();
    supervisorRun.join();
    return 0;
}
