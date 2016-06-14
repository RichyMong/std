#include "bigint.h"
#include <iostream>

using namespace std;

int main()
{
    util::BigInt bi(339912500);
    cout << bi << endl;
    cout << bi.to_int64() << endl;
}
