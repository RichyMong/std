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

#define MEASURE_CALL(_statement...)                                                    \
    do {                                                                               \
        auto start_tp = steady_clock::now();                                           \
        _statement;                                                                  \
        auto d = steady_clock::now() - start_tp;                                       \
        cout << "use " << duration_cast<microseconds>(d).count() << " microseconds\n"; \
    } while(0)

template <typename K, typename V>
static void key_intersect(const map<K, V>& one, const map<K, V>& another,
        vector<K>& same, vector<K>& in_one, vector<K>& in_another)
{
    auto transformer = [] (const auto& value) { return value.first; };

    vector<K> one_vec, another_vec;
    MEASURE_CALL(
        transform(one.begin(), one.end(), back_inserter(one_vec), transformer);
        transform(another.begin(), another.end(), back_inserter(another_vec), transformer)
    );

    MEASURE_CALL(set_intersection(one_vec.begin(), one_vec.end(),
                     another_vec.begin(), another_vec.end(),
                     back_inserter(same))
                );

    MEASURE_CALL(set_difference(one_vec.begin(), one_vec.end(),
                   another_vec.begin(), another_vec.end(),
                   back_inserter(in_one))
                );

    MEASURE_CALL(set_difference(another_vec.begin(), another_vec.end(),
                     one_vec.begin(), one_vec.end(),
                     back_inserter(in_another))
                );
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

    MEASURE_CALL(
        set_intersection(one_beg, one_end, another_beg, another_end, back_inserter(same))
    );
    MEASURE_CALL(
        set_difference(one_beg, one_end, another_beg, another_end, back_inserter(in_one))
    );
    MEASURE_CALL(
        set_difference(another_beg, another_end, one_beg, one_end, back_inserter(in_another))
    );
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

template <typename K, typename V>
void map_same_diff(const map<K, V>& one, const map<K, V>& another)
{
    vector<K> same, in_one, in_another;
    {
        PrintPeriod period;
        key_intersect(one, another, same, in_one, in_another);
    }

    vector<K> same_tf, in_one_tf, in_another_tf;
    {
        PrintPeriod period;
        key_intersect_transform(one, another, same_tf, in_one_tf, in_another_tf);
    }

    assert(same == same_tf);
    assert(in_one == in_one_tf);
    assert(in_another == in_another_tf);

    cout << "same " << same.size() << endl;
    cout << "in one " << in_one.size() << endl;
    cout << "in another " << in_another.size() << endl;
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
    static constexpr int MIN_ELEMENT_SIZE = 10;

    size_t nr_elements = MIN_ELEMENT_SIZE;
    if (argc > 1) {
        nr_elements = max(MIN_ELEMENT_SIZE, atoi(argv[1]));
    }

    if (false) {
        map<string, int> one, another;
        while (one.size() < nr_elements) {
            one[rand_string()] = one.size();
        }

        while (another.size() < nr_elements) {
            another[rand_string()] = another.size();
        }

        map_same_diff(one, another);
    } else {
        map<int, int> one, another;
        while (one.size() < nr_elements) {
            one[rand()] = one.size();
        }

        while (another.size() < nr_elements) {
            another[rand()] = another.size();
        }

        map_same_diff(one, another);

    }

    return 0;
}
