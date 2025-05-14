# 🗨️ C++ Chat System

A multi-client chat system built using C++ and Winsock, where:

- Clients and server communicate using JSON format via TCP sockets.
- Clients can send messages to the server.
- The server can broadcast or send messages to specific clients (by ID or IP).
- Supports LAN and internet-based messaging (via port forwarding or Ngrok).

---

## 📁 Project Structure
chat/
├── client/
│ ├── Client.cpp
│ └── Client.h
├── server/
│ ├── Server.cpp
│ └── Server.h
├── json.hpp // nlohmann::json (single-header library)
├── main_client.cpp // Entry point for client
├── main_server.cpp // Entry point for server
├── README.md
└── .gitignore

## ⚙️ Features

- JSON-based messaging (using [nlohmann/json](https://github.com/nlohmann/json))
- Broadcast and private message support
- Multithreaded server using `std::thread`
- Client identification using Client ID or IP address
- Works across devices on the same Wi-Fi or public internet (via port forwarding)

---

## 🚀 How to Build

### 🛠 Prerequisites

- Windows OS
- [Visual Studio Build Tools](https://visualstudio.microsoft.com/visual-cpp-build-tools/)
- Developer Command Prompt (comes with VS)

### 📦 Compilation

1. Open `x64 Native Tools Command Prompt` (or Developer Command Prompt).
2. Navigate to your project folder.
```sh
cd C:\Users\your-name\Desktop\chat
3.Compile the server
   cl /EHsc main_server.cpp server\Server.cpp /I. /link ws2_32.lib
4.Compile the client:
   cl /EHsc main_client.cpp client\Client.cpp /I. /link ws2_32.lib

📡 How to Run
1.Start the server (on your PC):
  main_server.exe
2.Start client(s) (on same PC or LAN device)
  main_client.exe

📥 Sample Input Format
From Server to specific client by ID:
      /send 2 Hello Client 2!
From Server to all clients:
      Hello everyone!



