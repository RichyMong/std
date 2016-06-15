#include <stdio.h>
#include <stdlib.h>

int main()
{
    int a;
    int *p = NULL, *q = NULL, *r = &a;
    int *t = (p || q || r) || NULL;
    a = 8;

    print
}
