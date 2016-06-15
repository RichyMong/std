#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <set>
#include <assert.h>
#include "../utils/util.h"

using namespace std;

template <class Container>
auto make_set(const Container& c) -> set<typename Container::value_type>
{
    return set<typename Container::value_type>(c.begin(), c.end());
}

template <class ContainerA, class ContainerB>
void assert_equal(const ContainerA& a, const ContainerB& b)
{
    assert(make_set(a) == make_set(b));
}

vector<int> make_unique_ints(int size)
{
    random_device rd;
    uniform_int_distribution<int> dist(1, size * 10);

    set<int> result;

    while (result.size() != size) {
        int v = dist(rd);
        if (result.find(v) == result.end()) {
            result.insert(v);
        }
    }

    vector<int> vec(result.begin(), result.end());
    std::random_shuffle(vec.begin(), vec.end());
    return vec;
}

void compare(const vector<int>& a, const vector<int>& b,
    vector<int>& in_both, vector<int>& in_a, vector<int>& in_b)
{
    vector<int> newa(a), newb(b);
    sort(newa.begin(), newa.end());
    sort(newb.begin(), newb.end());
    std::set_intersection(newa.begin(), newa.end(), newb.begin(), newb.end(), back_inserter(in_both));
    std::set_difference(newa.begin(), newa.end(), newb.begin(), newb.end(), back_inserter(in_a));
    std::set_difference(newb.begin(), newb.end(), newa.begin(), newa.end(), back_inserter(in_b));
}

void vector_compare(const vector<int>& a, const vector<int>& b,
    vector<int>& in_both,vector<int>& in_a, vector<int>& in_b)
{
    for (int i = 0; i < a.size(); ++i) {
        if (find(b.begin(), b.end(), a[i]) != b.end()) {
            in_both.push_back(a[i]);
        } else {
            in_a.push_back(a[i]);
        }
    }
    for (int i = 0; i < b.size(); ++i) {
        if (find(a.begin(), a.end(), b[i]) == a.end()) {
            in_b.push_back(b[i]);
        }
    }
}

bool stl_find(set<int>& s, int v)
{
    auto it = s.find(v);
    if (it != s.end()) {
        s.erase(it);
    }

    return true;
}

bool linear_find(set<int>& s, int v)
{
    auto it = s.begin();
    for (; it != s.end(); ++it) {
        if (*it == v)  break;
    }

    if (it != s.end()) {
        s.erase(v);
    }
    return true;
}

int main(int argc, char* argv[])
{
    constexpr int SIZE = 10000;
    int size = (argc > 1) ? std::stoi(argv[1]) : SIZE;
    auto a = make_unique_ints(size);
    auto b = make_unique_ints(size);
    vector<int> in_both, in_a, in_b;

    using CT = const vector<int>;
    using T = vector<int>;
    auto p = util::timeit(compare, a, b, in_both, in_a, in_b);
    cout << "set compare time: " << p.count() << "us\n";
    vector<int> vin_both, vin_a, vin_b;
    auto q = util::timeit(vector_compare, a, b, vin_both, vin_a, vin_b);
    cout << "vector compare time: " << q.count() << "us\n";
    assert_equal(in_both, vin_both);
    assert_equal(in_a, vin_a);
    assert_equal(in_b, vin_b);

    set<int> s(a.begin(), a.end());
    auto find_result = util::timeit(stl_find, s, a.back());
    cout << "stl_find use " << find_result.second.count() << '\n';
    auto linear_result = util::timeit(linear_find, s, a.back());
    cout << "linear_find use " << linear_result.second.count() << '\n';

}
