#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static void* signal_thread(void *data);
static void* worker_thread(void *data);

static bool running = true;

int main()
{
    sigset_t sigset;
    pthread_t t1, t2;

    sigfillset(&sigset);
    sigdelset(&sigset, SIGSTOP);
    sigdelset(&sigset, SIGKILL);

    pthread_sigmask(SIG_BLOCK, &sigset, NULL);
    pthread_create(&t1, NULL, signal_thread, NULL);
    pthread_create(&t2, NULL, worker_thread, NULL);

    pthread_join(t2, NULL);
    pthread_join(t1, NULL);
}

static void check_mask()
{
    sigset_t mask;
    pthread_sigmask(SIG_BLOCK, NULL, &mask);
    pthread_mutex_lock(&lock);
    printf("thread id: %llu\n", (unsigned long long) pthread_self());
    if (sigismember(&mask, SIGINT))
        printf("sigint is member\n");
    if (sigismember(&mask, SIGQUIT))
        printf("sigquit is member\n");
    pthread_mutex_unlock(&lock);
}

static void sigint(int signo)
{
    printf("sigint handler\n");
}

static void* signal_thread(void *data)
{
    int signo;
    sigset_t sigset;
    sigset_t set;

    sleep(2);
    check_mask();
    sigfillset(&sigset);
    pthread_sigmask(SIG_UNBLOCK, NULL, &set);
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGINT);
    sigaddset(&sigset, SIGQUIT);
    // sigaddset(&sigset, SIGSTOP);
    signal(SIGINT, sigint);

    // though we install a handler for SIGINT, it seems that Linux
    // chooses to invoke sigwait instead of call the handler.
    sigwait(&sigset, &signo);
    printf("signal %s\n", strsignal(signo));
    running = false;

    return NULL;
}

static void* worker_thread(void *data)
{
    check_mask();
    while (running) {
        sleep(1);
    }
    return data;
}
