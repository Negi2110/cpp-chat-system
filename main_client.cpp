#include "client/Client.h"

int main() {
    Client client("127.0.0.1", 8080);
    client.start();

    while (true) std::this_thread::sleep_for(std::chrono::seconds(60));
}
