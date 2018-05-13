#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <string>
#include <fcntl.h>

#define BUFFER_SIZE 1024

int open_socket() {
    int listener;
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        perror("socket");
        exit(1);
    }

    int opt = 1;
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
    }

    return listener;
}

void binding(int listener, int port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(listener, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(2);
    }
}

int accepting(int listener) {
    int sock = accept(listener, nullptr, nullptr);
    if (sock < 0) {
        perror("accept");
        exit(3);
    }
    return sock;
}

std::string read_file(int fd) {
    std::string file;
    ssize_t read_count = 0;
    ssize_t reads = 0;
    char buffer[BUFFER_SIZE];
    while ((reads = read(fd, buffer, BUFFER_SIZE)) > 0) {
        file += buffer;
        read_count += reads;
    }
    file += "reserved";
    file[read_count] = '\0';
    return file;
}

void executor(int sock) {
    char buf[1024];
    ssize_t bytes = recv(sock, buf, BUFFER_SIZE, 0);
    buf[bytes] = '\0';
    int fd = open(buf, O_RDONLY);
    if (fd < 0) {
        char message[] = "Can't find file.";
        send(sock, message, sizeof(message), 0);
        return;
    }

    std::string file = read_file(fd);
    close(fd);
    send(sock, file.data(), file.length(), 0);
}

void execute(int sock) {
    executor(sock);
    close(sock);
}

int main(int argc, char **argv) {
    int port = 3425;
    if (argc == 2) {
        port = atoi(argv[1]);
    }

    int listener;
    listener = open_socket();
    binding(listener, port);

    listen(listener, 1);

    // 5 requests
    for (int i = 0; i < 5; ++i) {
        int sock = accepting(listener);
        execute(sock);
    }

    close(listener);
    return 0;
}