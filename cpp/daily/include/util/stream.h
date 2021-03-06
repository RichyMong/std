# pragma once

#include <cstddef>
#include <vector>
#include <string>
#include <fstream>
#include <exception>
#include <type_traits>
#include <string.h>
#include <assert.h>
#include "types.h"

namespace util {

template <typename T> class InputStream;

struct SubsFailure {};

template <typename T>
struct SubsResult : public std::true_type {};

template <>
struct SubsResult<SubsFailure> : public std::false_type {};

template <typename T, typename X>
class GetUpdateResult
{
    template <typename U, void (U::*)(const InputStream<X>&)> struct SFINAE {  };
    template <typename U> static void check(SFINAE<U, &U::template update<InputStream<X>>>*);
    template <typename U> static SubsFailure check(...);

public:
    using type = decltype(check<T>(0));
};

template <typename T, typename X>
struct HasUpdate : public SubsResult<typename GetUpdateResult<T, X>::type>
{
};

class OverReadException : public std::exception
{
public:
    OverReadException(int len, int offset, int readable)
    {
        snprintf(buffer_, sizeof(buffer_), "try to read %d uint8_ts at %d:%d", len, offset, readable);
    }

    virtual const char* what() const throw()
    {
        return buffer_;
    }

public:
    char buffer_[128];
};

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

        ::memcpy(buffer_ + offset_, &x, sizeof(_Type));

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

        ::memcpy(buffer_ + offset_, x, needed);
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
    uint8_t buffer_[_BufSize];
    mutable size_t offset_;
};

class InputBuffer {
public:
    InputBuffer(const uint8_t* buffer, size_t size)
        : buffer_(buffer), size_(size)
    {
    }

    size_t readable() const {
        return size_ - offset_;
    }

    size_t offset() const {
        return offset_;
    }

    void read(uint8_t* buffer, size_t len) const
    {
        if (len > readable()) {
            throw OverReadException(len, offset_, size_);
        }

        ::memcpy(buffer, buffer_ + offset_, len);
    }

    void skip(int n) { offset_ += n; }

private:
    const uint8_t* buffer_;
    const size_t size_;
    mutable size_t offset_;
};

class InputFile
{
public:
    explicit InputFile(const std::string& path)
        : fs_ { path, std::ios::binary | std::ios::in }
    {
        auto pos = fs_.tellg();
        fs_.seekg(0, std::ios_base::end);
        size_ = fs_.tellg() - pos;
        fs_.seekg(pos, std::ios_base::beg);
    }

    size_t offset() const {
        return const_cast<std::fstream&>(fs_).tellg();
    }

    size_t readable() const {
        return size_ - offset();
    }

    void read(uint8_t* buffer, size_t len) const
    {
        if (len > readable()) {
            throw OverReadException(len, offset(), size_);
        }

        const_cast<std::fstream&>(fs_).read((char*) buffer, len);
    }

    void skip(int n) { fs_.seekg(n, std::ios_base::cur); }

private:
    std::fstream fs_;
    size_t size_;
};

template <typename InputType>
class InputStream
{
public:
    template <typename... Args>
    InputStream(Args... args)
        : input_ { std::forward<Args...>(args...) }
    {
    }

    size_t readable() const {
        return input_.readable();
    }

    size_t offset() const { return input_.offset(); }

    bool eof() const { return !readable(); }

#define READ_FUNCTION(type)                 \
    type read_##type() const {              \
        assert(readable() >= sizeof(type)); \
        type x;                             \
        read(x);                            \
        return x;                           \
    }

    // Helper functions. The caller must be sure readable() >= sizeof(type)
    READ_FUNCTION(char);
    READ_FUNCTION(int);
    READ_FUNCTION(short);
    READ_FUNCTION(long);
    READ_FUNCTION(uint8_t);
    READ_FUNCTION(uint16_t);
    READ_FUNCTION(uint32_t);
    READ_FUNCTION(uint64_t);

    template <typename _Type,
        typename=typename std::enable_if<std::is_pod<_Type>::value>::type>
    _Type read() const
    {
        _Type x;
        read(x);
        return x;
    }

    template <typename _SizeType>
    std::string read_string() const {
        std::string x;
        read<_SizeType>(x);
        return x;
    }

    template <typename DataType,
        typename NotUsed = typename std::enable_if<!HasUpdate<DataType, InputType>::value, int>::type,
        typename=typename std::enable_if<std::is_pod<DataType>::value>::type>
    void read(DataType& x, NotUsed = NotUsed()) const
    {
        if (readable() < sizeof(DataType)) {
            throw_overread(sizeof(DataType));
        }

        uint8_t buffer[sizeof(DataType)];
        input_.read(buffer, sizeof(buffer));
        ::memcpy(&x, buffer, sizeof(DataType));
    }

    template <typename DataType,
        typename NotUsed = typename std::enable_if<HasUpdate<DataType, InputType>::value, int>::type>
    void read(DataType& x, NotUsed = NotUsed()) const
    {
        if (readable() < sizeof(DataType)) {
            throw_overread(sizeof(DataType));
        }

        x.update(*this);
    }

    template <typename _ElementType, size_t _Size,
              typename std::enable_if<std::is_pod<_ElementType>::value>::type>
    void read(_ElementType (&x)[_Size])
    {
        auto needed = _Size * sizeof(_ElementType);
        if (needed > readable()) {
            throw_overread(needed);
        }

        input_.read((char*) &x, needed);
    }

    // Read into a std::string or std::vector<T>
    template <typename _SizeType = unsigned short, typename _ElementType,
             template <typename...> class Container,
          typename=typename std::enable_if<std::is_pod<_ElementType>::value>::type,
        typename=typename std::enable_if<std::is_integral<_SizeType>::value>::type>
    void read(Container<_ElementType>& container) const
    {
        _SizeType size = read<_SizeType>();
        if (sizeof(_ElementType) * size > readable()) {
            throw_overread(sizeof(_ElementType) * size);
        }

        container.reserve(size);
        for (_SizeType i = 0; i < size; i++) {
            container.push_back(read<_ElementType>());
        }
    }

private:
    void throw_overread(int size) const
    {
        throw OverReadException(size, offset(), readable());
    }

    InputType input_;
};

using InputBufferStream = InputStream<InputBuffer>;
using InputFileStream = InputStream<InputFile>;

} // namespace util
