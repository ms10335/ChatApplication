#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <chrono>
#include <fstream>
#include <mutex>
#include <string>

#define BUFLENGTH 1024
namespace fs = std::filesystem;

class Client {
private:
    int client_socket;
    sockaddr_in client_hint;
    void create_socket();
    void create_connection();
    void handleInformation();
    void fileReceived();

public:
    Client() {
        std::cout << "Start Client Session\n";
        create_socket();
        create_connection();
        handleInformation();
    }
};

void Client::create_socket() {
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "Socket is not created properly\n";
        exit(EXIT_FAILURE);
    }
}
void Client::create_connection() {
    int port = 54000;
    std::string ipAddress = "127.0.0.1";
    client_hint.sin_family = AF_INET;
    client_hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &client_hint.sin_addr);
    int connetRes = connect(client_socket, (sockaddr*)&(client_hint), sizeof(client_hint));
    if (connetRes == -1) {
        std::cerr << "Cannot connect to to server socket\n";
        exit(EXIT_FAILURE);
    }
    std::cout << "Connect to server...\n";
}
void Client::handleInformation() {
    int bytesRead = 0;
    int bytesWrite = 0;
    char buf[BUFLENGTH];
    auto time_start = std::chrono::steady_clock::now();
    while (true) {
        std::cout << '>';
        std::string data;
        getline(std::cin, data);
        memset(buf, 0, BUFLENGTH);
        if (data == "exit") {
            bytesWrite += send(client_socket, data.c_str(), data.size(), 0);
            break;
        }
        if (data == "send") {
            bytesWrite += send(client_socket, data.c_str(), data.size(), 0);
            fileReceived();

            continue;
        }
        bytesWrite += send(client_socket, data.c_str(), data.size(), 0);
        std::cout << "Awaiting server response...\n";
        memset(buf, 0, BUFLENGTH);
        bytesRead += recv(client_socket, buf, BUFLENGTH, 0);
        if (!strcmp(buf, "exit")) {
            std::cout << "Server has quit the session\n";
            break;
        }
        std::cout << "Server: " << buf << '\n';
    }
    auto time_end = std::chrono::steady_clock::now();
    close(client_socket);
    std::cout << "****** CLIENT SESSION *****\n";
    std::cout << "Bytes writen: " << bytesWrite << '\n';
    std::cout << "Bytes read: " << bytesRead << '\n';
    std::cout << "Elapse time : " << std::chrono::duration_cast<std::chrono::seconds>(time_end - time_start).count() << "sec\n";
}
void Client::fileReceived() {
    std::string file_path = "/home/marcins/Data/Client/client_file.txt";
    std::fstream file;
    file.open(file_path, std::ios::out | std::ios::app | std::ios::binary);
    if (file.is_open()) {
        std::cout << "File is ready to receive\n";
    } else {
        std::cout << "File is not exist\n";
        return;
    }
    char buf[BUFLENGTH] = {};
    int fileRead = read(client_socket, buf, BUFLENGTH);
    std::cout << "Data received: " << fileRead << " bytes\n";
    file << buf;
    file << '\n';
    file.close();
}

int main() {
    Client client;

    return 0;
}