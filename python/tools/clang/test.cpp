#include <vector>
#include <string>
#include "test.h"

class Foo
{
    int data_;

public:
    Foo(std::vector<int>&){}

    void setData(std::string data);

    void setData(std::vector<int> data);

    void setData(std::vector<char> data);
private:
    void setLocalData();
};

void Foo::setData(std::string data)
{
   // do nothing
}

void Foo::setData(std::vector<int> data)
{
   // do nothing
}
