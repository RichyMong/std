#include <boost/iterator/transform_iterator.hpp>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;
using namespace boost;
using namespace boost::iterators;

template <typename K, typename V>
struct KeyGetter
{
    const K& operator()(const pair<K, V>& value) const { return value.first; }
};

int main()
{
    typedef map<int, int> IntMap;
    typedef KeyGetter<IntMap::key_type, IntMap::mapped_type> KeyFunction;
    typedef boost::iterators::transform_iterator<KeyFunction, IntMap::iterator> KeyIterator;

    IntMap mi { {1, 1}, {2, 4}, {3, 9} };

    KeyFunction func;
    KeyIterator bmi = make_transform_iterator(mi.begin(), func);
    KeyIterator emi = make_transform_iterator(mi.end(), func);
    copy(bmi, emi, ostream_iterator<int>(std::cout, " "));

    cout << endl;

    vector<int> vi;
    map<int, int> ami { {1, 3}, {3, 9} };
    std::set_intersection(bmi, emi,
         make_transform_iterator(ami.begin(), func),
         make_transform_iterator(ami.end(), func),
         back_inserter(vi));

    for (size_t i = 0; i < vi.size(); i++) {
        cout << vi[i] << ' ';
    }

    cout << endl;

    return 0;
}
