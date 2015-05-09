#include <iostream>
#include <string>
#include <memory>
#include <vector>

using namespace std;

class AsynCall {
public:
    explicit AsynCall(const std::string& name)
        : name_{ name }
    {
    }

    void greeting()
    {
        cout << "hello, " << name_ <<  endl;
    }

    ~AsynCall() { cout << "byebye, " << name_ << endl; }

private:
    string name_;
};

vector<std::function<void(void)>> calls;

void add_call(const std::string& name)
{
    auto asycall = make_shared<AsynCall>(name);
    calls.push_back(bind(&AsynCall::greeting, asycall));
}

int main()
{
    add_call("test");
    cout << "add test call\n";
    for (auto& functor : calls) {
        functor();
    }
}
