#include "test.h"

class Foo
{
    int data_;

public:
    Foo(){}

    void setData(int data);

    void setData(char data);

    void setData(const char*);
    void setData(const char*);
    void setData(const char*);
private:
    void setLocalData();
};

#endif
