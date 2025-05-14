#include "server/Server.h"

int main() {
    Server server(8080);
    server.start();

    while (true) std::this_thread::sleep_for(std::chrono::seconds(60));
}
