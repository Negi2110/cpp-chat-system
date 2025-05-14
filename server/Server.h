#pragma once
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <winsock2.h>

class Server {
public:
    explicit Server(int port);
    ~Server();
    void start();
    void stop();

private:
    int port;
    SOCKET listenSocket;
    bool running;

    std::mutex clients_mutex;
    std::unordered_map<std::string, SOCKET> ipToSocket;

    void acceptClients();
    void handleClient(SOCKET clientSocket, std::string clientIP);
    void inputThread();
    void sendToIP(const std::string& ip, const std::string& message);
    void broadcastMessage(const std::string& message);
};
