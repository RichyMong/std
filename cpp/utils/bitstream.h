# pragma once

#include <algorithm>
#include <stddef.h>
#include <string>
#include <sstream>

namespace util {

template  <size_t SIZE, bool USE_LITTLE = false>
class BitStream {
public:
    BitStream()
        : nbits_ {0},
          offset_ { 0 }
    {
    }

    void write(unsigned int n, int bits)
    {
        n &= (1 << bits) - 1;

        for (int i = 0; i < bits; ) {
            int idx = current_byte();
            int used = current_used();
            int nwrite = std::min(BYTE_BITS - used, bits - i);
            int mask = (1 << (used + nwrite)) - (1 << used);

            // We need to write the highest nwrite bits of buffer_[idx]
            buffer_[idx] &= ~mask;

            int value = 0;
            if (USE_LITTLE) {
                // #(bits - i) bits of n is available and we want to put the highest
                // # nwrite  bits of it at buffer_[idx] starting from #used.
                value  = n >> (bits - i - nwrite);
            } else {
                value = n >> i;
            }

            buffer_[idx] |= (value << used) & mask;
            i += nwrite;
            offset_ += nwrite;
        }

        if (nbits_ < offset_) nbits_ = offset_;
    }

    int read(int bits) const
    {
        int n = 0;
        for (int i = 0; i < bits; ) {
            int idx = current_byte();
            int used = current_used();
            int nread = std::min(BYTE_BITS - used, bits - i);
            int mask = (1 << (used + nread)) - (1 << used);
            int value = (buffer_[idx] & mask) >> used;

            if (USE_LITTLE) {
                n = (n << nread) | value;
            } else {
                // highest remain bits of buffer_[idx] is used to construct the
                // lowest remain bits of n.
                n |= value << i;
            }
            i += nread;
            offset_ += nread;
        }

        return n;
    }

    void append(int n, int bits)
    {
        seek(nbits_);
        write(n, bits);
    }

    template <typename T, size_t N>
    void write(const T (&array)[N])
    {
        char* raw = (char *) array;

        for (int i = 0; i < N * sizeof(T); ++i) {
            write(raw[i], BYTE_BITS);
        }
    }

    template <typename T, size_t N>
    void read(T (&array)[N]) const
    {
        char* raw = (char*) array;

        for (int i = 0; i < N * sizeof(T); ++i) {
            raw[i] = read(BYTE_BITS);
        }
    }

    std::string dump() const
    {
        std::ostringstream oss;

        for (int i = 0; i < size(); ++i) {
            int remain = BYTE_BITS;
            if (i == size() - 1) {
                remain = nbits_ % 8;
            }
            for (int j = 0; j < remain; ++j) {
                oss << ((buffer_[i] & (1 << j)) ? 1 : 0);
            }

            if ((i + 1) % 5 == 0) oss << '\n';
            else oss << ' ';
        }

        oss << '\n';

        return oss.str();
    }

    int capacity() const { return SIZE * BYTE_BITS; }

    int nbits() const { return nbits_; }

    int current_byte() const { return offset_ / BYTE_BITS; }

    int current_used() const { return offset_ % BYTE_BITS; }

    int size() const { return (nbits_ + BYTE_BITS - 1) / BYTE_BITS; }

    void seek(size_t offset) { offset_ = offset; }

private:
    static constexpr int BYTE_BITS = 8;

    unsigned char buffer_[SIZE];
    int nbits_;
    mutable int offset_;
};

}
