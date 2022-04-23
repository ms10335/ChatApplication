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


class Server {
private:
    std::fstream file;
    int socket_descriptor;
    int clientSocket;
    sockaddr_in server_hint;
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];

    void create_socket();
    void bind_socket();
    void set_listen();
    void accept_connection();
    void transmit_file();
    void handleInforamtion();

public:
    Server() {
        std::cout << "Start Server Session:\n";
        create_socket();
        bind_socket();
        set_listen();
        accept_connection();
        handleInforamtion();
    }
};

void Server::create_socket() {
    socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_descriptor == -1) {
        std::cerr << "Failed to connect to server\n";
        exit(EXIT_FAILURE);
    }
}
void Server::bind_socket() {
    server_hint.sin_family = AF_INET;
    server_hint.sin_port = htons(54000);
    inet_pton(AF_INET, "0.0.0.0", &server_hint.sin_addr);
    if (bind(socket_descriptor, (sockaddr*)&server_hint, sizeof(server_hint)) == -1) {
        std::cerr << "Failed to bind socket\n";
        exit(EXIT_FAILURE);
    }
}
void Server::set_listen() {
    if (listen(socket_descriptor, SOMAXCONN) == -1) {
        std::cerr << "Cannot listen on specyfic port\n";
        exit(EXIT_FAILURE);
    }
}
void Server::accept_connection() {
    clientSocket = accept(socket_descriptor, (sockaddr*)&client, &clientSize);
    if (clientSocket == -1) {
        std::cerr << "Cannot establish connection with client\n";
    }
    // close the listening server socket
    close(socket_descriptor);
    memset(host, 0, NI_MAXHOST);
    memset(svc, 0, NI_MAXSERV);

    int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);

    if (result) {
        std::cout << host << " conected on " << svc << '\n';
    } else {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        std::cout << host << " conected on " << ntohs(client.sin_port) << '\n';
    }
}
void Server::transmit_file() {
    namespace fs = std::filesystem;
    std::string file_path = "/home/marcins/Data/Server/server.txt";
    std::fstream file;
    fs::path p = file_path;
    file.open(file_path, std::ios_base::in | std::ios_base::binary);
    if(file.is_open()) {
        std::cout << "File is ready to send\n";
    }else {
        std::cout << "File is not exist\n";
        return;
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::cout << "Data to transimit" << content.size() << "bytes\n";  
    std::cout << "Sending...\n";
    int bytesFilesSend = send(clientSocket, content.c_str(), content.length(), 0);
    std::cout << "Bytes sent: " << bytesFilesSend <<'\n';
    file.close();
}
void Server::handleInforamtion() {
    auto time_now = std::chrono::steady_clock::now();
    int bytesRead = 0;
    int bytesWrite = 0;
    char buf[4096];
    while (true) {
        // rec messaga from client
        memset(buf, 0, 4096);
        bytesRead += recv(clientSocket, buf, 4096, 0);
        if (bytesRead == -1) {
            std::cerr << "There was a connecion issue\n";
            break;
        }
        if (bytesRead == 0) {
            std::cerr << "Client is disconected\n";
            break;
        }
        if (!strcmp(buf, "exit")) {
            std::cerr << "Client has disconected\n";
            break;
        }
        if(! strcmp(buf, "send")) {
            transmit_file();
            continue;
        }
        std::cout << "Client: " << buf << '\n';
        std::cout << ">";
        std::string answer;
        getline(std::cin, answer);
        memset(buf, 0, 4096);
        strcpy(buf, answer.c_str());
        if (answer == "exit") {
            // closed conection by serever
            send(clientSocket, buf, strlen(buf), 0);
            break;
        }
        // send the messag to client
        bytesWrite += send(clientSocket, buf, strlen(buf), 0);
    }
    auto time_end = std::chrono::steady_clock::now();
    close(clientSocket);
    std::cout << "\t**********SERVER SESSION**********\n";
    std::cout << "Bytes written: " << bytesWrite << '\n';
    std::cout << "Bytes read: " << bytesRead << '\n';
    std::cout << "Elapse time : " << std::chrono::duration_cast<std::chrono::seconds>(time_end - time_now).count() << "sec\n";
}

int main() {
    Server ss;
}