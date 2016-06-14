#include <list>
#include <vector>
#include <map>
#include <set>
#include <string>

using namespace std;

struct POD {
    int x;
    int y;
};

int main(int argc, const char *argv[])
{
    (void) argc;
    (void) argv;

    std::vector<int> vi { 10, 20, 30, 40 };
    std::list<char> lc { 'a', 'b', 'x', 'y' };
    std::map<int, POD> mp { { 1, { 2, 3 } }, { 4, { 5, 6 } } };
    std::set<string> ss { "hello", "world" };

    return 0;
}
