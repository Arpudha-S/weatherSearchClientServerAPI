#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>

using namespace std;

#pragma comment(lib, "ws2_32.lib")
const int PORT = 8081;
const char* SERVER_IP = "127.0.0.1";
const int BUFFER_SIZE = 1024;

int main() {
    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE] = { 0 };

    // Initialize Winsock
    cout<<"Client Application started\n";
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed" <<endl;
        return 1;
    }

    // Create socket
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
        cerr << "Socket creation failed" <<endl;
        WSACleanup();
        return 1;
    }
    cout<<"Client Socket started\n";

    // Prepare the sockaddr_in structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddr.sin_port = htons(PORT);

    // Connect to server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Connection failed" <<endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    cout << "Connected to server" <<endl;
    string weather;
    cout << "Use this format to enter data:\n\n  set_cityname_temperature\n  get_city_cityname\n  get_all\n  get_equal_temperature\n  get_greater_temperature\n  get_less_temperature\n\n";

    cin >> weather;

    // Send message to server
    send(clientSocket, weather.c_str(), strlen(weather.c_str()), 0);
    cout << "Message sent to server" <<endl;

    // Receive message from server
    int valread = recv(clientSocket, buffer, BUFFER_SIZE, 0);
    cout << "Received message from server: " << buffer <<endl;
    
    system("pause");
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
