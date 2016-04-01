#include <zlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
    if (argc < 2) {
        return 1;
    }

    unsigned char buf[4196];
    unsigned long destlen = sizeof(buf);
    int rc = decompress2(buf, &destlen, message, strlen(message), 9);
    if (rc == Z_OK) {
        printf("compressed len: %lu\n", destlen);
        for (int i = 0; i < destlen; ++i) {
            printf("%02X", buf[i]);
        }
        printf("\n");
    } else {
        printf("compress failed: %d\n", rc);
    }

    return 0;
}
