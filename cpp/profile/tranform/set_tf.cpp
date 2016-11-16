#include <iostream>
#include <chrono>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <cstdio>
#include <assert.h>
#include <boost/iterator/transform_iterator.hpp>

using namespace std;
using namespace std::chrono;
using namespace boost;

template <typename K, typename V>
static void key_intersect(const map<K, V>& one, const map<K, V>& another,
        vector<K>& same, vector<K>& in_one, vector<K>& in_another)
{
    vector<K> one_vec, another_vec;
    one_vec.reserve(one.size());
    for (auto it = one.begin(); it != one.end(); ++it) {
        one_vec.push_back(it->first);
    }

    another_vec.reserve(another.size());
    for (auto it = another.begin(); it != another.end(); ++it) {
        another_vec.push_back(it->first);
    }

    set_intersection(one_vec.begin(), one_vec.end(),
                     another_vec.begin(), another_vec.end(),
                     back_inserter(same));

    set_difference(one_vec.begin(), one_vec.end(),
                   another_vec.begin(), another_vec.end(),
                   back_inserter(in_one));

    set_difference(another_vec.begin(), another_vec.end(),
                   one_vec.begin(), one_vec.end(),
                   back_inserter(in_another));
}

template <typename K, typename V>
static void key_intersect_transform(const map<K, V>& one, const map<K, V>& another,
        vector<K>& same, vector<K>& in_one, vector<K>& in_another)
{
    using value_type = typename map<K, V>::value_type;
    auto key_func = [](const value_type& value) { return value.first; };

    auto one_beg = iterators::make_transform_iterator(one.begin(), key_func);
    auto one_end = iterators::make_transform_iterator(one.end(), key_func);
    auto another_beg = iterators::make_transform_iterator(another.begin(), key_func);
    auto another_end = iterators::make_transform_iterator(another.end(), key_func);

    set_intersection(one_beg, one_end, another_beg, another_end, back_inserter(same));
    set_difference(one_beg, one_end, another_beg, another_end, back_inserter(in_one));
    set_difference(another_beg, another_end, one_beg, one_end, back_inserter(in_another));
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
        auto d = duration_cast<microseconds>(steady_clock::now() - first_tp_);
        fprintf(stdout, "used %ld microseconds\n", d.count());
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

    map<int, int> one, another;
    while (one.size() < nr_elements) {
        one[rand() % high] = 1;
    }

    while (another.size() < nr_elements) {
        another[rand() % high] = 1;
    }

    vector<int> same_tf, in_one_tf, in_another_tf;
    PrintPeriod period;
    key_intersect_transform(one, another, same_tf, in_one_tf, in_another_tf);

    return 0;
}
