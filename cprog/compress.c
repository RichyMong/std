#include <zlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
    const char* message = (argc > 1) ? argv[1] : "Hello, I am here!";
    printf("begin to compress \"%s\"\n", message);

    unsigned char buf[4196];
    unsigned long destlen = sizeof(buf);
    int rc = compress2(buf, &destlen, (unsigned char*)message, strlen(message), 9);
    if (rc == Z_OK) {
        printf("compressed len: %lu\n", destlen);
        for (unsigned long i = 0; i < destlen; ++i) {
            printf("%02X", buf[i]);
        }
        printf("\n");
    } else {
        printf("compress failed: %d\n", rc);
    }

    return 0;
}
