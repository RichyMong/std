# pragma once

#include <cstddef>
#include <vector>
#include <string>
#include <type_traits>
#include <string.h>
#include "types.h"

namespace util {

template <size_t _BufSize>
class OutputBufferStream {
public:
    OutputBufferStream()
        : offset_ { 0 }
    {
    }

    size_t writeable() const {
        return _BufSize - offset_;
    }

    template <typename _Type,
        typename=typename std::enable_if<std::is_pod<_Type>::value && !std::is_pointer<_Type>::value>::type>
    bool write(_Type x)
    {
        if (writeable() < sizeof(_Type)) {
            return false;
        }

        memcpy(buffer_ + offset_, &x, sizeof(_Type));

        offset_ += sizeof(_Type);
        return true;
    }

    template <typename _ElementType, size_t _Size,
              typename=typename std::enable_if<std::is_pod<_ElementType>::value>::type>
    bool write(const _ElementType (&x)[_Size])
    {
        auto needed = _Size * sizeof(_ElementType);
        if (needed > writeable()) {
            return false;
        }

        memcpy(buffer_ + offset_, x, needed);
        offset_ += needed;

        return true;
    }

    template <typename _SizeType = unsigned short, typename _ElementType,
              template <typename...> class Container,
              typename=typename std::enable_if<std::is_integral<_SizeType>::value>::type>
    bool write(const Container<_ElementType>& container)
    {
        auto needed = sizeof(_SizeType) + sizeof(_ElementType) * container.size();
        if (needed > writeable()) {
            return false;
        }

        write(static_cast<_SizeType>(container.size()));
        for (size_t i = 0; i < container.size(); i++) {
            write(container[i]);
        }

        return true;
    }

public:
    byte buffer_[_BufSize];
    mutable size_t offset_;
};

class InputBufferStream {
public:
    InputBufferStream(const byte* buffer, size_t size)
        : buffer_(buffer), size_(size)
    {
    }

    size_t readable() const {
        return size_ - offset_;
    }

    char readChar() const {
        char c;
        read(c);

        return c;
    }

    int readInt() const {
        int n;

        read(n);

        return n;
    }

    int readShort() const {
        short n;

        read(n);

        return n;
    }

    template <typename _SizeType>
    std::string readString() const {
        std::string x;
        read<_SizeType>(x);
        return x;
    }

    template <typename _Type,
        typename=typename std::enable_if<std::is_pod<_Type>::value>::type>
    bool read(_Type& x) const
    {
        if (readable() < sizeof(_Type)) {
            return false;
        }

        memcpy(&x, buffer_ + offset_, sizeof(_Type));

        offset_ += sizeof(_Type);
        return true;
    }

    template <typename _ElementType, size_t _Size,
              typename std::enable_if<std::is_pod<_ElementType>::value>::type>
    bool read(_ElementType (&x)[_Size])
    {
        auto needed = _Size * sizeof(_ElementType);
        if (needed > readable()) {
            return false;
        }

        memcpy(buffer_ + offset_, x, needed);
        offset_ += needed;

        return true;
    }

    // Read into a std::string or std::vector<T>
    template <typename _SizeType = unsigned short, typename _ElementType,
             template <typename...> class Container,
          typename=typename std::enable_if<std::is_pod<_ElementType>::value>::type,
        typename=typename std::enable_if<std::is_integral<_SizeType>::value>::type>
    bool read(Container<_ElementType>& container) const
    {
        _SizeType size;
        if (!read(size)) {
            return false;
        }

        if (sizeof(_ElementType) * size > readable()) {
            return false;
        }

        container.reserve(size);
        auto start = (_ElementType *) (buffer_ + offset_);
        std::copy(start, start + size, std::back_inserter(container));
        offset_ += sizeof(_ElementType) * size;

        return true;
    }

private:
    const byte* buffer_;
    const size_t size_;
    mutable size_t offset_;
};

}
