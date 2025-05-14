#include "Server.h"
#include <iostream>
#include <algorithm>
#include <ws2tcpip.h>
#include "../json.hpp"

using json = nlohmann::json;
#pragma comment(lib, "ws2_32.lib")

Server::Server(int port) : port(port), running(false) {
    WSAStartup(MAKEWORD(2, 2), new WSADATA());
}

Server::~Server() {
    stop();
    WSACleanup();
}

void Server::start() {
    listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(listenSocket, SOMAXCONN);
    running = true;

    std::cout << "[Server] Listening on port " << port << "...\n";

    std::thread(&Server::inputThread, this).detach();
    std::thread(&Server::acceptClients, this).detach();
}

void Server::stop() {
    running = false;
    closesocket(listenSocket);
}

void Server::acceptClients() {
    while (running) {
        sockaddr_in clientAddr;
        int addrlen = sizeof(clientAddr);
        SOCKET clientSocket = accept(listenSocket, (sockaddr*)&clientAddr, &addrlen);
        if (clientSocket != INVALID_SOCKET) {
            char ipStr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(clientAddr.sin_addr), ipStr, INET_ADDRSTRLEN);
            std::string clientIP = ipStr;

            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                ipToSocket[clientIP] = clientSocket;
            }

            std::cout << "[Server] Client connected from " << clientIP << "\n";
            std::thread(&Server::handleClient, this, clientSocket, clientIP).detach();
        }
    }
}

void Server::handleClient(SOCKET clientSocket, std::string clientIP) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int valread = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (valread <= 0) {
            std::cout << "[Client " << clientIP << "] Disconnected.\n";
            std::lock_guard<std::mutex> lock(clients_mutex);
            ipToSocket.erase(clientIP);
            closesocket(clientSocket);
            break;
        }

        try {
            auto received = json::parse(buffer);
            std::string msg = received.value("message", "");
            std::cout << "[Client " << clientIP << "] " << msg << "\n";
        } catch (...) {
            std::cout << "[Client " << clientIP << "] Invalid JSON\n";
        }
    }
}

void Server::broadcastMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (auto& pair : ipToSocket) {
        send(pair.second, message.c_str(), message.length(), 0);
    }
}

void Server::sendToIP(const std::string& ip, const std::string& message) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    if (ipToSocket.find(ip) != ipToSocket.end()) {
        send(ipToSocket[ip], message.c_str(), message.length(), 0);
    } else {
        std::cout << "[Error] No client with IP: " << ip << "\n";
    }
}

void Server::inputThread() {
    std::string input;
    while (true) {
        std::cout << "[You]: ";
        std::getline(std::cin, input);
        if (input.empty()) continue;

        if (input.rfind("/send ", 0) == 0) {
            size_t spacePos = input.find(' ', 6);
            if (spacePos != std::string::npos) {
                std::string ip = input.substr(6, spacePos - 6);
                std::string msg = input.substr(spacePos + 1);

                json j;
                j["server"] = "Admin";
                j["message"] = msg;
                sendToIP(ip, j.dump());
                continue;
            }
        }

        json j;
        j["server"] = "Admin";
        j["message"] = input;
        broadcastMessage(j.dump());
    }
}
