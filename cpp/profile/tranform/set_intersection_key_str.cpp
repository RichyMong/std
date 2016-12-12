#include <iostream>
#include <chrono>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <cstdio>
#include <assert.h>
#include <boost/iterator/transform_iterator.hpp>
#include "../include/comfun.h"
#include "../../utils/iterator.h"

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
}

template <typename K, typename V>
static void key_intersect_transform(const map<K, V>& one, const map<K, V>& another,
        vector<K>& same, vector<K>& in_one, vector<K>& in_another)
{
    using value_type = typename map<K, V>::value_type;
    auto key_func = [](const value_type& value) { return value.first; };

    auto one_beg = util::key_iterator(one.begin());
    auto one_end = util::key_iterator(one.end());
    auto another_beg = util::key_iterator(another.begin());
    auto another_end = util::key_iterator(another.end());

    set_intersection(one_beg, one_end, another_beg, another_end, back_inserter(same));
    // set_difference(one_beg, one_end, another_beg, another_end, back_inserter(in_one));
    // set_difference(another_beg, another_end, one_beg, one_end, back_inserter(in_another));
}

string rand_string()
{
    char buf[6];

    int v = rand();
    sprintf(buf, "%c%c%c%c%c", v & 0x7f, (v >> 8) & 0x7f,
             (v >> 16) & 0x7f, (v >> 24) & 0x7f, (v >> (rand() % 30)) & 0x7f);

    return buf;
}

int main(int argc, char* argv[])
{
    size_t nr_elements = 0;
    if (argc > 1) nr_elements = atoi(argv[1]);
    if (nr_elements < 10) nr_elements = 10;

    int high = nr_elements * 2;

    map<string, int> one, another;
    while (one.size() < nr_elements) {
        one[rand_string()] = one.size();
    }

    while (another.size() < nr_elements) {
        another[rand_string()] = another.size();
    }

    vector<string> same, in_one, in_another;
    {
        PrintPeriod period("convert");
        key_intersect(one, another, same, in_one, in_another);
    }

    vector<string> same_tf, in_one_tf, in_another_tf;
    {
        PrintPeriod period("transform");
        key_intersect_transform(one, another, same_tf, in_one_tf, in_another_tf);
    }

    assert(same == same_tf);
    assert(in_one == in_one_tf);
    assert(in_another == in_another_tf);

    return 0;
}
