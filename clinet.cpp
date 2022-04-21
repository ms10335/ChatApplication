#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <signal.h>
#include <mutex>
#include <string>

bool exit_flag = false;
std::thread th_send, th_recv;
int client_socket;
void send_message(int clinet_socket);
void recv_message(int clinet_socket);

int main() {

    //create  a soscket
    return 0;
}