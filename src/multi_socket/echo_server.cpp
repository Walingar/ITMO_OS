#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <string>
#include <fcntl.h>
#include <vector>
#include <string>
#include <cstring>
#include <set>

#define BUFFER_SIZE 1024
#define EPOLL_SIZE 1000
#define EPOLL_RUN_TIMEOUT -1

int open_socket() {
    int listener;
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        perror("socket");
        exit(-1);
    }

    int opt = 1;
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        exit(-1);
    }

    return listener;
}

void set_non_blocking(int listener) {
    if (fcntl(listener, F_SETFL, fcntl(listener, F_GETFD, 0) | O_NONBLOCK) < 0) {
        perror("fcntl");
        exit(-1);
    }
}

void binding(int listener, int port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(listener, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind");
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

void listening(int listener) {
    if (listen(listener, 1) < 0) {
        perror("listen");
        exit(-1);
    }
}

void add_listener_to_epoll(int epoll, int listener, epoll_event &event) {
    if (epoll_ctl(epoll, EPOLL_CTL_ADD, listener, &event) < 0) {
        perror("epoll_ctl");
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

int accepting(int listener) {
    int sock = accept(listener, nullptr, nullptr);
    if (sock < 0) {
        perror("accept");
        exit(-1);
    }
    return sock;
}

void add_client_to_epoll(int epoll, int client, epoll_event &event) {
    if (epoll_ctl(epoll, EPOLL_CTL_ADD, client, &event) < 0) {
        perror("epoll_ctl");
        exit(-1);
    }
}

void client_closed_connection(int client) {
    printf("Client %d closed connection\n", client);
    if (close(client) < 0) {
        perror("close");
        exit(-1);
    }
}

void resend_message(int client) {
    char message[BUFFER_SIZE];
    bzero(message, BUFFER_SIZE);
    ssize_t len;
    if ((len = recv(client, message, BUFFER_SIZE, 0)) < 0) {
        perror("recv");
        exit(-1);
    }
    message[len] = '\0';
    if (len == 0) {
        client_closed_connection(client);
    } else {
        send(client, message, (size_t) len, 0);
    }
}

void send_welcome_message(int client) {
    std::string message = "Welcome to echo server!\n";
    send(client, message.data(), BUFFER_SIZE, 0);
}

int main(int argc, char *argv[]) {
    int port = 3425;
    if (argc == 2) {
        port = atoi(argv[1]);
    }

    int listener = open_socket();
    set_non_blocking(listener);
    binding(listener, port);

    listening(listener);
    int epoll = create_epoll(EPOLL_SIZE);

    static struct epoll_event event, events[EPOLL_SIZE];

    event.events = EPOLLIN | EPOLLET;
    event.data.fd = listener;
    add_listener_to_epoll(epoll, listener, event);

    printf("Waiting...\n");
    for (int i = 0; i < 100; ++i) {
        int epoll_events_count = waiting(epoll, events, EPOLL_SIZE, EPOLL_RUN_TIMEOUT);
        for (int cur_event = 0; cur_event < epoll_events_count; ++cur_event) {
            if (events[cur_event].data.fd == listener) {
                int client = accepting(listener);
                printf("Connected client:%d\n", client);
                set_non_blocking(client);
                event.data.fd = client;
                add_client_to_epoll(epoll, client, event);
                send_welcome_message(client);
            } else {
                resend_message(events[cur_event].data.fd);
            }
        }
    }

    close(listener);
    close(epoll);

    return 0;
}