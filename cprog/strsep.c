#include <stdio.h>
#include <string.h>

int main()
{
    char fields[] = "abc def ghi\tjkl";
    char *org = fields;
    char *ret;

    while ((ret = strsep(&org, " \t"))) {
        printf("%s\n", ret);
    }

    return 0;
}
