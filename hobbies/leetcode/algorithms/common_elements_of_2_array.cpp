#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace std::chrono;

using VI = vector<int>;
using Iter = VI::iterator;

void find_common_helper(Iter s1, Iter e1, Iter s2, Iter e2, VI& result)
{
    while (true) {
        if (s1 == e1 || s2 == e2) return;

        if (*s1 == *s2)  {
            result.push_back(*s1);
            ++s1, ++s2;
        } else {
            if (*s1 > *s2) {
                swap(s1, s2);
                swap(e1, e2);
            }

            s1 = lower_bound(s1, e1, *s2);
        }
    }
}

void find_common_helper2(Iter s1, Iter e1, Iter s2, Iter e2, VI& result)
{
    if (s1 == e1 || s2 == e2) return;

    if (*s1 == *s2)  {
        result.push_back(*s1);
        ++s1, ++s2;
    } else {
        if (*s1 > *s2) {
            swap(s1, s2);
            swap(e1, e2);
        }

        s1 = lower_bound(s1, e1, *s2);
    }

    if (s1 == e1 || s2 == e2) return;

    auto l1 = e1 - 1;
    auto l2 = e2 - 1;
    if (*l1 == *l2)  {
        result.push_back(*l1);
        --e1, --e2;
    } else {
        if (*l1 < *l2) {
            swap(s1, s2);
            swap(e1, e2);
        }

        l1 = lower_bound(s1, e1, *e2);
        e1 = l1 + 1;
    }

    find_common_helper(s1, e1, s2, e2, result);
}

VI find_common(const VI& va, const VI& vb)
{
    int i = 0, j = 0;
    VI result;

    while (i < va.size() && j < vb.size()) {
        if (va[i] == vb[j]) {
            result.push_back(va[i]);
            ++i, ++j;
        } else if (va[i] < vb[j]) {
            ++i;
        } else {
            ++j;
        }
    }

    return result;
}

VI find_common1(VI& va, VI& vb)
{
    VI result;
    find_common_helper(va.begin(), va.end(), vb.begin(), vb.end(), result);
    return result;
}

VI find_common2(VI& va, VI& vb)
{
    VI result;
    find_common_helper2(va.begin(), va.end(), vb.begin(), vb.end(), result);
    return result;
}

int generate_x(int x)
{
    if (x % 2 == 0) {
        return x + (rand() % 256);
    } else if (x % 3 == 0) {
        return x + (rand() % 512);
    } else if (x % 5 == 0) {
        return x + (rand() % 1024);
    }

    return x + (rand() % 4096);
}

int main(int argc, char** argv)
{
    constexpr int default_n = 100;

    int n = argc > 1 && atoi(argv[1]) > default_n ? atoi(argv[1]) : default_n;

    VI va;
    VI vb;

    for (int i = 0; i < n; ++i)
        va.push_back(generate_x(i + 1));

    for (int i = 0; i < n; ++i)
        vb.push_back(generate_x(i + 1));

    sort(va.begin(), va.end());
    sort(vb.begin(), vb.end());

    auto start_time = steady_clock::now();

    auto r1 = find_common(va, vb);

    cout << "use " << duration_cast<microseconds>(steady_clock::now() - start_time).count() << " microseconds\n";
    start_time = steady_clock::now();

    auto r2 = find_common1(va, vb);
    cout << "use " << duration_cast<microseconds>(steady_clock::now() - start_time).count() << " microseconds\n";

    if (r1 != r2) cout << "error\n";

    return 0;
}
