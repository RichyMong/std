#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    char addr[] = "192.168.1.102";

    for (char *pos = addr, *str = addr; pos = strsep(&str, "."); ) {
        printf("%d\n", atoi(pos));
    }

    return 0;
}
