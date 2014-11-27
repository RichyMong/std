#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

int main()
{
    cout.fill('0');
    cout.width(4);
    cout << 10 << endl;
    cout << setfill('#') << setw(4) << 10 << endl;
    cout << setw(4) << 10 << endl;

    ostringstream ss;
    ss << setfill('*') << setw(4) << 10;
    cout << ss.str() << endl;
    ss.str(string());
    ss << 20;
    cout << ss.str() << endl;
}
