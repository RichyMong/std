#include <stdio.h>
#include <pthread.h>

static int x, y[10];

/* The two following functions are running concurrently: */

void* sum_a(void* a)
{
    int i;
    for (i = 0; i < 1000000; ++i)
        x += i;
    return NULL;
}

void* inc_b(void* arg)
{
    int i;
    for (i = 0; i < 1000000; ++i)
        y[i%10] = i;
    return NULL;
}

int main()
{
    pthread_t ta, tb;
    printf("&x=%p, &y=%p\n", &x, &y);
    pthread_create(&ta, NULL, sum_a, NULL);
    pthread_create(&tb, NULL, inc_b, NULL);
}
