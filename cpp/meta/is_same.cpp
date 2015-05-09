#include <iostream>
#include <type_traits>

using namespace std;

template <typename T, typename U>
struct is_same_type : public std::false_type {

};

template <typename T>
struct is_same_type<T, T> : public std::true_type {

};

int main() {
    cout << is_same_type<int, int>() << endl;
    cout << is_same_type<int, short>::value << endl;
}
