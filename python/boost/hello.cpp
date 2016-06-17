#include <iostream>
#include <string>
#include <boost/python.hpp>

const char* greet()
{
    return "hello, world";
}

struct Overload {
    static void f(int x) { std::cout << "int " << x << '\n'; }
    static void f(const std::string& x) { std::cout << "string " << x << '\n'; }
};

BOOST_PYTHON_MODULE(hello_ext)
{
    using namespace boost::python;
    class_<Overload>("Overload")
        .def("f", (void(*)(int))Overload::f)
        .def("f", (void(*)(const std::string&))Overload::f)
        .staticmethod("f")
        ;
}
