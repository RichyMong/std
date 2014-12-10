#include <stdio.h>
#include "module.h"

extern void app_call();

int main(int argc, char *argv[])
{
    printf("module1 : main\n");
    app_call();

    register_module("module1");

    return 0;
}
