#include "replace_type.h"
#include <type_traits>
#include <iostream>

int main() {
    using namespace std;
    using namespace ouou;

    cout << is_same<long*, replace_type<int*, int, long>::type>::value << endl;
    cout << is_same<int[5], replace_type<char[5], char, int>::type>::value << endl;
}
