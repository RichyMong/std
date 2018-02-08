#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main()
{
    char buf[4096];
    int fd = open("/proc/self/cmdline", O_RDONLY);
    if (fd < 0) {
        exit(1);
    }
    ssize_t len = read(fd, buf, sizeof(buf));
    close(fd);
    if (len < 0) {
        exit(1);
    }

    char *start = strchr(buf, '\0');
    char *end = buf + len;
    if (start != NULL && start + 1 != end) {
        for (char *pos = start + 1; pos < end; ++pos) {
            if (*pos == 0) {
                *pos = ' ';
            }
        }
        printf("%s\n", start + 1);
    }
    return 0;
}
