#include <vector>
#include <iostream>
#include <map>
#include <chrono>
#include <string>
#include <sstream>
#include "iterator.h"

using namespace std;
using namespace std::chrono;
using namespace util;

template <class Iterator>
void doIt(Iterator begin, Iterator end)
{
    ostringstream oss;
    while (begin != end) {
        oss << *begin++;
    }
}

int main(int argc, char* argv[])
{
    map<int, int> name_map { { 10, 1 }, { 100, 2 }, { 1000, 3 } };
    cout << distance(name_map.rbegin(), name_map.rend()) << endl;
    cout << distance(key_iterator(name_map.begin()), key_iterator(name_map.end())) << endl;
    vector<int> vi(key_iterator(name_map.begin()), key_iterator(name_map.end()));
    ostream_iterator<int> osi(cout, " ");
    copy(key_iterator(name_map.begin()), key_iterator(name_map.end()), osi);
    cout << "\n";

    const int size = (argc > 1) ? atoi(argv[1]) : 1000000;
    using clock_cls = high_resolution_clock;

    map<string, int> huge_map;
    for (int i = 0; i < size; ++i) {
        string str = "prefix" + to_string(i);
        huge_map[str] = i + 1;
    }

    auto start = clock_cls::now();
    {
        vector<int> converted;
        converted.reserve(huge_map.size());
        for (auto it = huge_map.begin(); it != huge_map.end(); ++it) {
            converted.push_back(it->second);
        }
        doIt(converted.begin(), converted.end());
    }
    auto end = clock_cls::now();
    cout << "map to vector method:" << duration_cast<nanoseconds>(end - start).count() << endl;
    start = clock_cls::now();
    doIt(value_iterator(huge_map.begin()), value_iterator(huge_map.end()));
    end = clock_cls::now();
    cout << "value iterator method:" << duration_cast<nanoseconds>(end - start).count() << endl;
    return 0;
}

