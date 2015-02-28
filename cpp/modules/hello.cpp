#include "module.h"
#include <iostream>

class Hello : public meng::Module {
public:
    explicit Hello(const std::string &host) : host_ {host} { }
    void work() override { std::cout << "host is " << host_ << std::endl; }

private:
    std::string host_;
};

static void __attribute__((constructor)) regist_hello()
{
    meng::ModuleManager::instance().regist("Hello",
                std::make_shared<Hello>("127.0.0.1"));
}
