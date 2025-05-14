#include "Client.h"
#include <iostream>
#include <ws2tcpip.h>
#include "../json.hpp"

using json = nlohmann::json;
#pragma comment(lib, "ws2_32.lib")

Client::Client(const std::string& host, int port)
    : host(host), port(port), running(false) {
    WSAStartup(MAKEWORD(2, 2), new WSADATA());
}

Client::~Client() {
    stop();
    WSACleanup();
}

void Client::start() {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed.\n";
        return;
    }

    running = true;
    std::cout << "[Connected to Server]\n";

    std::thread(&Client::sendMessages, this).detach();
    std::thread(&Client::receiveMessages, this).detach();
}

void Client::stop() {
    running = false;
    closesocket(sock);
}

void Client::sendMessages() {
    std::string input;
    while (running) {
        std::cout << "[You]: ";
        std::getline(std::cin, input);
        if (input.empty()) continue;

        json msg;
        msg["message"] = input;
        std::string data = msg.dump();
        send(sock, data.c_str(), data.length(), 0);
    }
}

void Client::receiveMessages() {
    char buffer[1024];
    while (running) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(sock, buffer, sizeof(buffer), 0);
        if (bytes <= 0) break;
        buffer[bytes] = '\0';
        std::cout << "\n[Server]: " << buffer << "\n";
    }
}
