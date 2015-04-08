#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <errno.h>

static const char* sem_path = "semfork";

int main() {
    sem_t* sem = sem_open(sem_path, O_CREAT | O_EXCL);
    if (sem == SEM_FAILED) {
        fprintf(stderr, "failed to create semphore %s: %s\n",
                sem_path, strerror(errno));
        exit(1);
    }

    int pid = fork();
    if (pid < 0) {
        exit(2);
    } else if (pid == 0) {
        printf("child waiting...\n");
        sem_wait(sem);
        printf("child continuing...\n");
    } else {
        printf("parent continuing\n");
        sleep(3);
        sem_post(sem);

        if (sem_unlink(sem_path) < 0) {
            perror("sem_unlink");
        }
    }
    if (sem_close(sem) < 0) {
        perror("sem_close");
    }
    exit(EXIT_SUCCESS);
}
