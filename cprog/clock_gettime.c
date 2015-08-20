#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void print_time()
{
    struct timespec now;
    if (clock_gettime(CLOCK_REALTIME, &now) < 0) {
        perror("get real time");
    }

    printf("real time: %ld.%ld\n", now.tv_sec, now.tv_nsec);

    if (clock_gettime(CLOCK_MONOTONIC, &now) < 0) {
        perror("get monotonic time");
    }

    printf("monotonic time: %ld.%ld\n", now.tv_sec, now.tv_nsec);
}

void sigint(int signo)
{
    (void) signo;

    print_time();

    exit(EXIT_SUCCESS);
}

int main()
{
    print_time();

    sleep(30);

    print_time();

    return 0;
}
