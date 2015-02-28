#include <stdio.h>

#define STR "hello"

int main(int argc, char *argv[])
{
    char buf[sizeof(STR) - 1];
    int wlen = snprintf(buf, sizeof(buf), "%s", STR);
    if (wlen >= (int)sizeof(buf)) {
        printf("buffer too small. Expected min len %d, buffer len %zu\n",
                wlen + 1, sizeof(buf));
    }
    printf("buffer %s\n", buf);
    return 0;
}
