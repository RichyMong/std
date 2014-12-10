#include <stdio.h>
#include <pthread.h>

static void* worker_thread(void *data);

int main(int argc, char *argv[])
{
    pthread_t worker;

    pthread_create(&worker, NULL, worker_thread, NULL);
    pthread_join(worker, NULL);

    return 0;
}

static void *worker_thread(void *data)
{
    printf("%s\n", __func__);

    return NULL;
}
