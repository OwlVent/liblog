// log_server.cpp - КОМПИЛИРОВАТЬ КАК ОТДЕЛЬНУЮ ПРОГРАММУ!
// Windows: g++ log_server.cpp -o log_server.exe -lws2_32
// Linux/macOS: g++ log_server.cpp -o log_server

#include <iostream>
#include <string>
#include <vector>

using namespace std;

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib") 
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    typedef int SOCKET; 
    const int INVALID_SOCKET = -1;
    const int SOCKET_ERROR = -1;
    #define closesocket close
#endif

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

void print_error(const char* message) {
#ifdef _WIN32
    cerr << message << " with error: " << WSAGetLastError() << endl;
#else
    perror(message);
#endif
}

int main() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed." << endl;
        return 1;
    }
#endif

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        print_error("Socket creation failed");
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Принимать подключения на любом IP-адресе
    serverAddr.sin_port = htons(PORT);

    if (bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        print_error("Bind failed");
        closesocket(listenSocket);
        return 1;
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        print_error("Listen failed");
        closesocket(listenSocket);
        return 1;
    }

    cout << "Log server is listening on port " << PORT << "..." << endl;

    while (true) {
        SOCKET clientSocket = accept(listenSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            print_error("Accept failed");
            continue; // Продолжаем ждать другие подключения
        }
        cout << "Client connected." << endl;

        char buffer[BUFFER_SIZE];
        int bytesReceived;
        while ((bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0)) > 0) {
            cout.write(buffer, bytesReceived);
        }

        if (bytesReceived == 0) {
            cout << "Client disconnected." << endl;
        } else {
            print_error("Recv failed");
        }
        closesocket(clientSocket);
    }

    closesocket(listenSocket);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}