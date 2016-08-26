#include <map>
#include <iostream>

using namespace std;

int main()
{
    std::map<int, int> imap { { 1, 1234 }, { 2, 4567 }, { 3, 8912 } };
    auto it = imap.find(3);
    if (it != imap.end()) {
        cout << imap[3] << endl;
    }

    return 0;
}


