# pragma once

#include <type_traits>
#include <assert.h>

namespace ouou {

template <class Target, class Source>
inline Target polymorphic_downcast(Source* x)
{
    assert(dynamic_cast<Target>(x) == x );
    return static_cast<Target>(x);
}

template <class Target, class Source>
inline Target polymorphic_downcast(Source& x)
{
    return *polymorphic_downcast<typename std::add_pointer<Target>::type>(&x);
}

}
