#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <iostream>
#include <sys/epoll.h>

#define BUFSIZE 1024
#define EPOLL_SIZE 1000
#define EPOLL_RUN_TIMEOUT -1
#define PIPE_SIZE 2

bool shutdown_client = false;

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

void set_up_pipe(int pipe_fd[PIPE_SIZE]) {
    if (pipe(pipe_fd) < 0) {
        perror("pipe");
        exit(-1);
    }
}

int create_epoll(int size) {
    int epfd = epoll_create(size);
    if (epfd < 0) {
        perror("epoll_create");
        exit(-1);
    }
    return epfd;
}

std::string get_request() {
    std::string request;
    std::getline(std::cin, request);
    return request;
}

void add_server_to_epoll(int epoll, int sock, epoll_event &event) {
    if (epoll_ctl(epoll, EPOLL_CTL_ADD, sock, &event) < 0) {
        perror("");
        exit(-1);
    }
}

void send_message(int to, const char *data, unsigned long size) {
    if (write(to, data, size) < 0) {
        perror("write");
        exit(-1);
    }
}

int waiting(int epoll, epoll_event *pEvent, int size, int time) {
    int count = epoll_wait(epoll, pEvent, size, time);
    if (count < 0) {
        perror("epoll_wait");
        exit(-1);
    }
    return count;
}

void server_closed_connection(int server) {
    printf("Client %d closed connection\n", server);
    if (close(server) < 0) {
        perror("close");
        exit(-1);
    }
}

void connected_to_server(int sock) {
    ssize_t len;
    char message[BUFSIZE];
    if ((len = recv(sock, message, BUFSIZE, 0)) < 0) {
        perror("recv");
        exit(-1);
    }
    message[len] = '\0';
    if (len == 0) {
        printf("ERROR: cannot connect to server\n");
        server_closed_connection(sock);
        shutdown_client = true;
    } else {
        printf("[SERVER]%s\n", message);
    }
}

int main(int argc, char **argv) {
    int port = 3425;
    if (argc == 2) {
        port = atoi(argv[1]);
    }
    int sock = open_socket();
    connecting(sock, port);

    int pipe_fd[PIPE_SIZE];
    set_up_pipe(pipe_fd);

    int epoll = create_epoll(EPOLL_SIZE);

    static struct epoll_event event, events[EPOLL_SIZE];
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = sock;

    add_server_to_epoll(epoll, sock, event);
    event.data.fd = pipe_fd[0];
    add_server_to_epoll(epoll, pipe_fd[0], event);

    connected_to_server(sock);
    int pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(-1);
    }
    switch (pid) {
        case 0:
            close(pipe_fd[0]);
            printf("Enter 'exit' to exit\n");
            while (!shutdown_client) {
                std::string request = get_request();
                if (request == "exit") {
                    shutdown_client = true;
                } else {
                    send_message(pipe_fd[1], request.data(), request.size());
                }
            }
            break;
        default:
            close(pipe_fd[1]);
            while (!shutdown_client) {
                int epoll_events_count = waiting(epoll, events, EPOLL_SIZE, EPOLL_RUN_TIMEOUT);
                for (int cur_event = 0; cur_event < epoll_events_count; ++cur_event) {
                    char message[BUFSIZE];
                    if (events[cur_event].data.fd == sock) {
                        ssize_t len;
                        if ((len = recv(sock, message, BUFSIZE, 0)) < 0) {
                            perror("recv");
                            exit(-1);
                        }
                        if (len == 0) {
                            server_closed_connection(sock);
                            shutdown_client = true;
                        } else {
                            printf("[SERVER]%s\n", message);
                        }
                    } else {
                        ssize_t len;
                        if ((len = read(events[cur_event].data.fd, message, BUFSIZE)) < 0) {
                            perror("read");
                            exit(-1);
                        }
                        message[len] = '\0';
                        if (len == 0) {
                            shutdown_client = true;
                        } else {
                            send(sock, message, (size_t) len, 0);
                        }
                    }
                }
            }
    }

    if (pid) {
        close(pipe_fd[0]);
        close(sock);
    } else {
        close(pipe_fd[1]);
    }
    return 0;
}