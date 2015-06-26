#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define err_exit(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

int start_server(int port)
{
    struct sockaddr_in addr;
    int    fd;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        err_exit("bind");

    if (listen(fd, SOMAXCONN) < 0)
        err_exit("listen");

    return fd;
}

void run_server(int port)
{
    int fd = start_server(port);
    while (true) {
        int client = accept(fd, NULL, NULL);
        char buf[129];
        int x;
        printf("clientfd: %d\n", client);
        struct linger linger;
        linger.l_onoff = 1;
        linger.l_linger = 0;
        setsockopt(client, SOL_SOCKET, SO_LINGER, &linger, sizeof(linger));
        close(client);
        continue;


        if ((x = read(client, buf, 128)) > 0) {
            buf[x] = 0;
            printf("%s\n", buf);
        }
        close(client);
    }
}

void add_epoll_event(int epfd, int fd, uint32_t events)
{
    struct epoll_event event;

    event.events = events | EPOLLET;
    event.data.fd = fd;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event) < 0)
        perror("epoll_ctl");
}

void setnonblock(int fd)
{
    int status;

    status = fcntl(fd, F_GETFL);
    status |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, status) < 0)
        err_exit("fcntl");
}

void run_client(int port)
{
    int epfd;
    int connfd;
    int rc;
    struct sockaddr_in addr;

    epfd = epoll_create(10);
    connfd = socket(AF_INET, SOCK_STREAM, 0);

    setnonblock(connfd);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_aton("127.0.0.1", &addr.sin_addr);

    rc = connect(connfd, (struct sockaddr*) &addr, sizeof(addr));
    if (rc < 0 && errno == EINPROGRESS)
        printf("connecting...\n");
    else if (rc == 0)
        printf("connected to server.\n");
    else {
        perror("connect");
    }

    add_epoll_event(epfd, connfd, EPOLLIN | EPOLLOUT);

    for ( ; ; ) {
        struct epoll_event events[32];
        int count = epoll_wait(epfd, events, 32, -1);
        int i;
        printf("%d events\n", count);
        for (i = 0; i < count; ++i) {
            int error;
            int rc;
            socklen_t len = sizeof(int);
            if (events[i].events & EPOLLOUT) {
                rc = getsockopt(events[i].data.fd, SOL_SOCKET, SO_ERROR, &error, &len);
                if (rc < 0)
                    perror("getsockopt");

                if (error == 0) {
                    printf("EPOLLOUT\n");
                } else {
                    fprintf(stderr, "could not connect to server: %s\n", strerror(error));
                }
                write(events[i].data.fd, "hi", 2);
            }
            if (events[i].events & EPOLLIN) {
                rc = getsockopt(events[i].data.fd, SOL_SOCKET, SO_ERROR, &error, &len);
                if (rc < 0)
                    perror("getsockopt");

                printf("EPOLLIN\n");
            }
            if (events[i].events & EPOLLERR) {
                printf("EPOLLERR\n");
            }
        }
    }
}

int main(int argc, char const* argv[])
{
    if (argc > 2) {
        run_client(atoi(argv[1]));
    } else {
        run_server(atoi(argv[1]));
    }

    return 0;
}
