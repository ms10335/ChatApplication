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
void send_message(const std::string&, int);
void send_message(int, int);
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
    //server_hint.sin_addr.s_addr = INADDR_ANY;
    inet_pton(AF_INET, "0.0.0.0", &server_hint.sin_addr);
    memset(&server_hint.sin_zero, 0, 8);

    if (bind(server_socket, reinterpret_cast<sockaddr*>(&server_hint), server_length) == -1) {
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
        if ((val_client_socket = accept(server_socket, reinterpret_cast<sockaddr*>(&client_socket), &client_size)) == -1) {
            std::cerr << "Error during accept conection!\n";
            return -1;
        }
        counter++;
        std::thread t(handle_client, val_client_socket, counter);
        std::lock_guard<std::mutex> guard(clients_mtx);
        clients.push_back({counter, "New one", val_client_socket, std::move(t)});
    }
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i].th.joinable()) {
            clients[i].th.join();
        }
    }

    // remebert to close
    close(server_socket);
    return 0;
}
void set_client_name(int id, const std::string& name) {
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i].id == id) {
            clients[i].name = name;
        }
    }
}
// send to all clients except sender
void send_message(const std::string& message, int client_id) {
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i].id != client_id) {
            send(clients[i].socket, message.c_str(), message.size(), 0);
        }
    }
}
// send a number to all clients except sender
void send_message(int number, int client_id) {
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i].id != client_id) {
            send(clients[i].socket, &number, sizeof(number), 0);
        }
    }
}
// how to close connection
void close_connection(int id) {
    for (size_t i = 0; i < clients.size(); ++i) {
        if (clients[i].id == id) {
            std::lock_guard<std::mutex> guard(clients_mtx);
            clients[i].th.detach();
            clients.erase(clients.begin() + i);
            close(clients[i].socket);
            break;
        }
    }
}

void handle_client(int client_socket, int clinet_id) {
    char name[MAX_INPUT];
    char str[MAX_INPUT];
    recv(client_socket, name, MAX_INPUT, 0);
    set_client_name(clinet_id, name);

    std::string welcome_msg = std::string(name) + " has joined";
    send_message(clinet_id, clinet_id);
    send_message(welcome_msg, clinet_id);

    while (true) {
        int bytes_received = recv(client_socket, str, MAX_INPUT, 0);
        if (bytes_received < 1) {
            return;
        }
        if (std::string(str) == "exit") {
            std::string bye_msg = std::string(name) + "has left the building!";
            send_message(clinet_id, clinet_id);
            send_message(bye_msg, clinet_id);
            close_connection(clinet_id);
            return;
        }
    }
    send_message(std::string(name), clinet_id);
    send_message(clinet_id, clinet_id);
    send_message(std::string(str),clinet_id);
    std::cout << '\n';
}
