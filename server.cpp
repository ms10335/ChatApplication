#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

struct client_term {
    int id;
    std::string name;
    int socket;
    std::thread th;
};
std::vector<client_term> clients;
std::mutex cout_mtx, clients_mtx;
int counter = 0;

void set_client_name(int, const std::string&);
void handle_client(int, int);
int send_message(const std::string&, int );
int send_message(int, int );
void close_connection(int);

int main() {
    // server socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        std::cerr << "Socket is established!\n";
        return -1;
    }

    // bind a server socket
    sockaddr_in server_hint;
    socklen_t server_length = sizeof(server_hint);
    server_hint.sin_family = AF_INET;
    server_hint.sin_port = htons(54000);
    server_hint.sin_addr.s_addr = INADDR_ANY;
    memset(&server_hint.sin_zero, 0, 8);

    if (bind(server_socket, reinterpret_cast<sockaddr*>(&server_hint), sizeof(server_length)) == -1) {
        std::cerr << "Failed to bind server socket!\n";
        return -1;
    }

    // mark socket for listening
    if (listen(server_socket, SOMAXCONN) == -1) {
        std::cerr << "Cannot listen on specified port!\n";
        return -1;
    }

    // accept call from client
    std::cout << "====== Welcome to the chat-room ======   " << '\n';
    sockaddr_in client_socket;
    socklen_t client_size = sizeof(client_socket);
    int val_client_socket{};
    while (true) {

        if((val_client_socket = accept(server_socket,reinterpret_cast<sockaddr*>(&client_socket), &client_size)) == -1) {
            std::cerr << "Error during accept conection!\n";
            return -1;
        }
        counter++;
        std::thread t(handle_client,client_socket,counter);
        std::lock_guard<std::mutex> guard(clients_mtx);
        clients.push_back({counter, "New one", val_client_socket, std::move(t)});

    }
    for (int i = 0; i < clients.size(); ++i) {
        if(clients[i].th.joinable()) {
            clients[i].th.join();
        }
    }
    
    //remebert to close 
    close(server_socket);
    return 0;
}