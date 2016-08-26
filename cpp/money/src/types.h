# pragma once

#include <cstddef>
#include <cstdint>
#include <iostream>

using byte = unsigned char;
using uint = unsigned int;
using ushort = unsigned short;
using ulong = unsigned long;

constexpr size_t kSmallBuf = 1 << 12;
constexpr size_t kMedianBuf = 1 << 16;
constexpr size_t kHugeBuf = 1 << 20;

class BigInt {
    friend std::ostream& operator<<(std::ostream& os, const BigInt& bi);
public:
    BigInt() = default;

    BigInt(int64_t val)
    {
        from_int64(val);
    }

    int64_t to_int64() const
    {
        int64_t val = base_;

        for (int i = 0; i < exp_; ++i) {
            val *= BASE;
        }

        return val;
    }

    operator int64_t() const { return to_int64(); }

private:
    void from_int64(int64_t val)
    {
        base_ = exp_ = 0;

        while (val > MAX_BASE || val < MIN_BASE) {
            val /= BASE;
            ++exp_;
        }

        base_ = val;
    }

    static constexpr int BASE_BITS = 29;
    static constexpr int EXP_BITS = 3;
    static constexpr int BASE = 16;
    static constexpr int64_t MAX_BASE = (1 << BASE_BITS) - 1;
    static constexpr int64_t MIN_BASE = -(1 << BASE_BITS);

    int32_t base_ : BASE_BITS;
    int32_t exp_ : EXP_BITS;
};

std::ostream& operator<<(std::ostream& os, const BigInt& bi)
{
    return os << bi.to_int64();
}

