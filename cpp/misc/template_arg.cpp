#include <iostream>
#include <memory>
#include <vector>
#include <list>
#include <string>

class X {
public:
    X(const std::string& name)
        : name_ { name }
    {
    }

    bool isMatch(const std::string& name) const { return name_ == name; }

private:
    std::string name_;
};

template <typename _ElementType, template <typename...> class _PtrType,
          template <typename...> class _ContainerType>
_ElementType* get(const _ContainerType<_PtrType<_ElementType>>& c, const std::string& name)
{
    for (auto &x : c) {
        if (x->isMatch(name)) {
            std::cout << "Found " << name << std::endl;
            return x.get();
        }
    }

    std::cout << "Cannot found " << name << std::endl;
    return nullptr;
}

int main()
{
    auto sp = std::make_shared<X>("one");
    std::vector<std::shared_ptr<X>> vec { sp };
    std::list<std::unique_ptr<X>> ls;
    ls.emplace_back(new X { "two" });
    get(vec, "one");
    get(vec, "two");
    get(ls, "one");
    get(ls, "two");
}
