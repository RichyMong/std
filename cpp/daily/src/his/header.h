# pragma once

#include <util/const.h>
#include <net/client.h>
#include <assert.h>

# pragma pack(push, 1)

struct Header
{
    uint8_t prefix;
    uint16_t msg_id;
    uint16_t flag;
    uint16_t data_size;
};

# pragma pack(pop)

class HeaderWrapper :  net::BaseHeader
{
public:
    virtual size_t size() const override
    {
        return HeaderWrapper::HEADER_SIZE;
    }

    virtual size_t pkg_size(const char* pkg, size_t len) const override
    {
        UNUSED_PARAM(len);

        assert(len >= HeaderWrapper::HEADER_SIZE);
        auto* h = reinterpret_cast<const Header*>(pkg);
        return  HeaderWrapper::HEADER_SIZE + h->data_size;
    }

    virtual bool is_complete(const char* pkg, size_t len) const override
    {
        if (len < HeaderWrapper::HEADER_SIZE) {
            return false;
        }
        auto* h = reinterpret_cast<const Header*>(pkg);
        return len >= HeaderWrapper::HEADER_SIZE + h->data_size;
    }

private:
    static constexpr size_t HEADER_SIZE = sizeof(Header);
};

