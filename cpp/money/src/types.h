# pragma once

#include <cstddef>
#include <cstdint>
#include <iostream>

using byte = unsigned char;
using uchar = unsigned char;
using uint = unsigned int;
using ushort = unsigned short;
using ulong = unsigned long;

constexpr size_t kSmallBuf = 1 << 12;
constexpr size_t kMedianBuf = 1 << 16;
constexpr size_t kHugeBuf = 1 << 20;

class LargeInt {
public:
    explicit LargeInt(int v = 0)
        : value_ { v }
    {
    }

    template <typename StreamType>
    void update(StreamType& is)
    {
        auto v = is.template read<int>();
        bool negative = false;
        if (v < 0) {
            negative = true;
            v = -v;
        }
        auto times = v >> 29;
        if (times) {
            v &= LargeInt::kFactor;
            v <<= times * 4;
        }

        value_ = negative ? -v : v;
    }

    template <typename StreamType>
    static LargeInt from_stream(StreamType& is)
    {
        LargeInt ret;
        ret.update(is);
        return ret;
    }

    operator int() const { return value_; }

private:
    static constexpr int kFactor = 0x1fffffff;

    int value_;
};

template <size_t NR_DIGITS>
class Digit {
public:
    Digit() = default;

    template <typename StreamType>
    void update(StreamType& is)
    {
        auto v = is.template read<int>();
        value_ = v / NR_DIGITS;
    }

    operator float() const { return value_; }

    template <typename StreamType>
    static Digit<NR_DIGITS> from_stream(StreamType& is)
    {
        Digit<NR_DIGITS> d;
        d.update(is);
        return d;
    }

private:
    float value_;
};

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

