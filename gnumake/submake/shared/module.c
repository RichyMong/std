#include <stdio.h>
#include <stdlib.h>
#include "module.h"

void register_module(const char *name)
{
    printf("registering %s\n", name);
}

void unregister_module(const char *name)
{
    printf("unregistering %s\n", name);
}
