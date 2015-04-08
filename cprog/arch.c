#include <stdio.h>

int main() {
#if (long) -1 == 0xFFFFFFFFFFFFFFFF
    printf("64 bits cpu\n");
#elif (long) -1 == 0xFFFFFFFF
    printf("32 bits cpu\n");
#endif

    return 0;
}
