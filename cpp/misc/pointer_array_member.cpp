#include <iostream>
#include <cstdio>
#include <cxxabi.h>

using namespace std;

template <typename E, size_t N>
void ArrayFunc(E (&ref)[N])
{
    cout << "ArrayFunc" << ": " << N << endl;
}

template <typename E, size_t N, typename T, E (T::*p)[N]>
static inline
E* ArrayConverter(T* data)
{
    int status;
    const E (&arrref)[N] = data->*p;
    const std::type_info  &ti = typeid(data->*p);
    auto realname = abi::__cxa_demangle(ti.name(), 0, 0, &status);
    cout << ti.name() << " : " << realname << endl;
    ArrayFunc(data->*p);
    return data->*p;
}

struct Info
{
    char name[32];
};

struct Book
{
    char name[12];
};

struct Numbers
{
    int data[3];
};

int main()
{
    Info info;
    sprintf(info.name, "info_name");

    cout << ArrayConverter<char, 32, Info, &Info::name>(&info) << endl;

    Book book;
    sprintf(book.name, "book_name");
    cout << ArrayConverter<char, 12, Book, &Book::name>(&book) << endl;

    Numbers numbers;
    numbers.data[0] = 1;
    numbers.data[1] = 3;
    numbers.data[2] = 6;

    auto x = ArrayConverter<int, 3, Numbers, &Numbers::data>(&numbers);
    cout << x[0] << " " << x[1] << " " << x[2] << endl;
}
