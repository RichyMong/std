#ifndef IS_BASE_AND_DERIVED_H
#define IS_BASE_AND_DERIVED_H

#include <type_traits>

namespace ouou {

template <class B, class D>
struct helper {
    operator B*() const;
    operator D*();
};

template <class B, class D>
class is_base_and_derived {
private:
    template <typename T>
    static char check(D*, T);

    static char (&check(B*, int))[2];

public:
    static const bool value =
        sizeof(check(helper<B, D>(), int())) == sizeof(char);
};

}

#endif // IS_BASE_AND_DERIVED_H
