#include <iostream>
#include <chrono>
#include <vector>
#include <map>
#include <fstream>
#include <thread>
#include <boost/iterator/transform_iterator.hpp>
#include "../../utils/iterator.h"

using namespace std;
using namespace std::chrono;
using namespace boost;
using namespace util;

template <typename Iterator>
static void do_work(Iterator first, Iterator last, size_t n = 0)
{
    fstream fs("test.txt", fstream::out);
    while (first != last) {
        fs << *first++;
    }
}

template <typename K, typename V>
static void key_intersect(const map<K, V>& one)
{
    vector<K> vec;
    vec.reserve(one.size());
    for (auto it = one.begin(); it != one.end(); ++it) {
        vec.push_back(it->first);
    }

    do_work(vec.begin(), vec.end(), vec.size());
}

template <typename K, typename V>
static void key_intersect_transform(const map<K, V>& one)
{
    using value_type = typename map<K, V>::value_type;
    auto key_func = [](const value_type& value) { return value.first; };

    auto one_beg = iterators::make_transform_iterator(one.begin(), key_func);
    auto one_end = iterators::make_transform_iterator(one.end(), key_func);
    do_work(one_beg, one_end, one.size());
}

template <typename K, typename V>
static void key_intersect_self(const map<K, V>& one)
{
    using value_type = typename map<K, V>::value_type;

    do_work(key_iterator(one.begin()), key_iterator(one.end()), one.size());
}

class PrintPeriod
{
public:
    PrintPeriod()
    {
        first_tp_ = steady_clock::now();
    }

    ~PrintPeriod()
    {
        auto d = duration_cast<nanoseconds>(steady_clock::now() - first_tp_);
        fprintf(stdout, "used %ld nanoseconds\n", d.count());
    }

private:
    steady_clock::time_point first_tp_;
};


template <typename T>
ostream& operator<<(ostream& os, const vector<T>& vec)
{
    for (auto &v : vec) {
        os << v << ' ';
    }
    return os << '\n';
}

int main(int argc, char* argv[])
{
    size_t nr_elements = 0;
    if (argc > 1) nr_elements = atoi(argv[1]);
    if (nr_elements < 10) nr_elements = 10;

    int high = nr_elements * 2;

    map<int, int> one;
    while (one.size() < nr_elements) {
        one[rand() % high] = 1;
    }

    {
        PrintPeriod period;
        key_intersect(one);
    }

    {
        PrintPeriod period;
        key_intersect_transform(one);
    }

    {
        PrintPeriod period;
        key_intersect_self(one);
    }

    return 0;
}
