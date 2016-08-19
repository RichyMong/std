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

bool operator==(const Overload& lhs, const Overload& rhs)
{
    return lhs.numbers_ == rhs.numbers_;
}

std::ostream& operator<<(std::ostream& os, const Overload& ol)
{
    os << "[";
    for (auto x : ol.numbers_) {
        os << " " << x;
    }
    os << "]";

    return os;
}

std::vector<Overload> get_vec()
{
    return std::vector<Overload> {  Overload(Dim{1,2,3}), Overload(Dim{11, 22, 33}) };
}

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
        .def(boost::python::self == boost::python::self)
        .def( self_ns::str(self_ns::self) )
        ;

    class_< std::vector< int > >( "IntVector" )
      .def( vector_indexing_suite< std::vector< int > >() );

    class_< std::vector< Overload > >( "OverloadVector" )
      .def( vector_indexing_suite< std::vector< Overload > >() );

    def("print", &print);
    def("get", &get);
    def("get_vec", &get_vec);
}
