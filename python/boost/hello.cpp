#include <iostream>
#include <string>
#include <vector>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

const char* greet()
{
    return "hello, world";
}

void print(const std::string& s)
{
    std::cout << 10 << std::endl;
    std::cout << s << std::endl;
}

struct Dim {
    int x, y, z;
};

struct Overload {
    Overload(const Dim& d)
    {
        std::cout << "constructing\n";
        numbers_.push_back(d.x);
        numbers_.push_back(d.y);
        numbers_.push_back(d.z);
    }

    static void f(int x) { std::cout << "int " << x << '\n'; }
    static void f(const std::string& x) { std::cout << "string " << x << '\n'; }

    std::vector<int> numbers_;
};

Overload get()
{
    return Overload(Dim{1,2,3});
}

BOOST_PYTHON_MODULE(hello)
{
    using namespace boost::python;
    class_<Overload>("Overload", init<const Dim&>())
        .def_readwrite("numbers_", &Overload::numbers_)
        .def("f", (void(*)(int))Overload::f)
        .def("f", (void(*)(const std::string&))Overload::f)
        .staticmethod("f")
        ;

    class_< std::vector< int > >( "IntVector" )
      .def( vector_indexing_suite< std::vector< int > >() );

    def("print", &print);
    def("get", &get);
}
