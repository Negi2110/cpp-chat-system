#pragma once
#include <string>
#include <thread>
#include <winsock2.h>

class Client {
public:
    Client(const std::string& host, int port);
    ~Client();
    void start();
    void stop();

private:
    std::string host;
    int port;
    SOCKET sock;
    bool running;

    void sendMessages();
    void receiveMessages();
};
