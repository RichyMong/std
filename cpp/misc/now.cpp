#include <iostream>
#include <string>
#include <time.h>

using namespace std;

enum class Test {
    T,
    V
};

int
main()
{
    char buf[128];
    struct tm now;
    time_t secs;

    time(&secs);

    localtime_r(&secs, &now);

    strftime(buf, sizeof(buf), "%F %T.", &now);

    cout << buf << endl;
}
