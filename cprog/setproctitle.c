#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

extern char **environ;

int main(int argc, char *argv[])
{
    int i;
    size_t size;
    char *p;
    char *last;

    printf("pid: %ld\n", getpid());
    size = 0;
    for (i = 0; environ[i]; ++i) {
        size += strlen(environ[i]) + 1;
    }

    last = argv[0];
    for (i = 0; argv[i]; ++i) {
        if (last == argv[i]) {
            last = argv[i] + strlen(argv[i]) + 1;
            printf("last: %p\n", last);
        }
    }

    p = (char *) malloc(size);
    for (i = 0; environ[i]; ++i) {
        if (environ[i] == last) {
            size = strlen(environ[i]) + 1;
            last = environ[i] + size;
            memcpy(p, environ[i], size);
            environ[i] = p;
            p += size;
        }
    }
    printf("argv[0]: %p, last: %p\n", argv[0], last);
    argv[1] = NULL;
    strncpy(argv[0], "proctitle: ", last - argv[0]);
    strncpy(argv[0] + strlen("proctitle: "), "test", last - argv[0] - strlen("proctitle: "));
    sleep(10);

    return 0;
}
