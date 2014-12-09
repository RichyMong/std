#include <stdio.h>

extern void app_call();

int main(int argc, char *argv[])
{
    printf("m1 : main\n");
    app_call();

    return 0;
}
