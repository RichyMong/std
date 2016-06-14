# pragma once
#include <cstdint>
#include <iostream>

namespace util {

class BigInt {
    friend std::ostream& operator<<(std::ostream& os, const BigInt& bi);
public:
    BigInt(int64_t val = 0)
    {
        from_int64(val);
    }

    int64_t to_int64() const
    {
        int64_t val = base_;

        for (int i = 0; i < exp_; ++i) {
            val *= BASE;
        }

        return val + mod_;
    }

    operator int64_t() const { return to_int64(); }

private:
    void from_int64(int64_t val)
    {
        base_ = exp_ = 0;
        mod_ = val % BASE;
        val -= mod_;

        while (val > MAX_BASE || val < MIN_BASE) {
            val /= BASE;
            ++exp_;
        }

        base_ = val;
    }

    static constexpr int BASE_BITS = 16;
    static constexpr int EXP_BITS = 8;
    static constexpr int MOD_BITS = 8;
    static constexpr int BASE = 1 << MOD_BITS;
    static constexpr int64_t MAX_BASE = (1 << BASE_BITS) - 1;
    static constexpr int64_t MIN_BASE = -(1 << BASE_BITS);

    int32_t base_ : BASE_BITS;
    int32_t exp_ : EXP_BITS;
    int32_t mod_ : MOD_BITS;
};

std::ostream& operator<<(std::ostream& os, const BigInt& bi)
{
    return os << "BigInt(base=" << bi.base_ << ", exponent=" << bi.exp_ <<
                 ", mod=" << bi.mod_ << ", value=" << bi.to_int64() << ")";
}

}
