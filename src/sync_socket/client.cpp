#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <iostream>

#define BUFSIZE 6000

int open_socket() {
    int sock;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
    }

    return sock;
}

void connecting(int sock, int port) {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("connect");
        exit(2);
    }
}

std::string get_request() {
    std::string request;
    printf("Enter file path: ");
    std::cin >> request;
    return request;
}

int main(int argc, char **argv) {
    int port = 3425;
    if (argc == 2) {
        port = atoi(argv[1]);
    }

    int sock = open_socket();
    connecting(sock, port);


    std::string message = get_request();
    send(sock, message.data(), message.length(), 0);

    char server_reply[BUFSIZE];
    if (recv(sock, server_reply, BUFSIZE, 0) < 0) {
        puts("recv failed");
    }
    puts("Reply received\n");
    puts(server_reply);

    close(sock);

    return 0;
}