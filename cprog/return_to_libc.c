#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	char buf[256];
	memcpy(buf, argv[1], strlen(argv[1]));
	printf(buf);

	return 0;
}
