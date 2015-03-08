#ifndef ROBOT_RELEASE_PACKET_H
#define ROBOT_RELEASE_PACKET_H

#include "util.h"
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <assert.h>
#include <string.h>

#ifdef __GNUC__
#define __MAY_ALIAS__        __attribute__((__may_alias__))
#else
#define __MAY_ALIAS__
#endif

#define PACKET_SIZE         0x8000
#define PACKET_DATA_SIZE    util::Packet::kDataLen
#define PACKET_MAGIC        0x5050
#define PACKET_LEN_BEGIN    4    // skip magic and crc

//////////////////////////////////////////////////////////////////////////

#define PACKET_FLAG_REPLY        0x0001
#define PACKET_FLAG_COMPRESS    0x0002
#define PACKET_FLAG_ENCRYPT        0x0004

//////////////////////////////////////////////////////////////////////////

// no byte order conversion
#define _HTONS(n)            (n)
#define _HTONL(n)            (n)
#define _HTONLL(n)            (n)

#define _NTOHS(n)            (n)
#define _NTOHL(n)            (n)
#define _NTOHLL(n)            (n)

//////////////////////////////////////////////////////////////////////////

namespace util {

class ExceptBase {

};

class BadPacket: public ExceptBase {

};

class PacketOverwrite: public ExceptBase {

};

class PacketOverread: public ExceptBase {

};

#pragma pack(push, 4)

struct PacketHdr {
    uint16        magic;
    uint16        flag;
    uint16         pktLen;
    uint16        mid;
    uint32        pid;

    uint16 getMagic() const {
        return _NTOHS(magic);
    }

    void setMagic(uint16 m) {
        magic = _HTONS(m);
    }

    uint16 getLen() const {
        return _NTOHS(pktLen);
    }

    void setLen(uint16 m) {
        pktLen = _HTONS(m);
    }

    uint16 getFlag() const {
        return _NTOHS(flag);
    }

    void setFlag(uint16 m) {
        flag = _HTONS(m);
    }

    uint16 getMid() const {
        return _NTOHS(mid);
    }

    void setMid(uint16 m) {
        mid = _HTONS(m);
    }

    uint32 getPid() const {
        return _NTOHL(pid);
    }

    void setPid(uint32 id) {
        pid = _HTONL(id);
    }
};

#pragma pack(pop)

class Packet {
public:
    virtual ~Packet() { }

    virtual void setData(const unsigned char* buf, uint16 len) = 0;

    uint16 getFlag() const {
        return ((PacketHdr* )(void *)_buf)->getFlag();
    }

    void setFlag(uint16 flag) {
        ((PacketHdr* )(void *)_buf)->setFlag(flag);
    }

    void setMsgId(uint16 mid) {
        PacketHdr& hdr = *(PacketHdr* )(void *)_buf;
        hdr.setMid(mid);
    }

    uint16 getMsgId() const {
        PacketHdr& hdr = *(PacketHdr* )(void *)_buf;
        return hdr.getMid();
    }

    uint32 getPid() const {
        PacketHdr& hdr = *(PacketHdr* )(void *)_buf;
        return hdr.getPid();
    }

    void setPid(uint32 id) {
        PacketHdr& hdr = *(PacketHdr* )(void *)_buf;
        hdr.setPid(id);
    }

    const char* getBuf() const {
        return _buf;
    }

    uint16 getBufSize() const {
        return _len;
    }

    const char* getData() const {
        return _buf + Packet::kHdrLen;
    }

    uint16 getDataSize() const {
        return _len > Packet::kHdrLen ? _len - Packet::kHdrLen : 0;
    }

    void setPos(uint16 pos) {
        _pos = pos > _len ? _len : pos;
    }

    uint16 getPos() const {
        return _pos;
    }

public:
    static const unsigned long kHdrLen = sizeof(PacketHdr);
    static const unsigned long kDataLen = PACKET_SIZE - sizeof(PacketHdr);

protected:
    Packet() {
    }

    Packet(const Packet& pkt) {
        memcpy(_buf, pkt.getBuf(), pkt.getBufSize());
        _pos = pkt._pos;
        _len = pkt._len;
    }

protected:
    char __MAY_ALIAS__  _buf[PACKET_SIZE];
    mutable uint16      _pos;
    uint16              _len;
};

class IPacket : public Packet {
public:
    IPacket() {
        reset();
    }

    ~IPacket(void) {

    }

    void reset() {
        PacketHdr& hdr = *(PacketHdr* )(void *)_buf;
        hdr.setMagic(0);
        hdr.pktLen = 0;
        hdr.flag = 0;
        hdr.mid = 0;
        hdr.pid = 0;

        _pos = Packet::kHdrLen;
        _len = 0;
    }

    uint16 append(const char* buf, size_t len) {
        uint16 ncopy = 0;

        assert(len > 0);

        if (_len < Packet::kHdrLen) {
            ncopy = std::min(len, Packet::kHdrLen - _len);
            memcpy(_buf + _len, buf, ncopy);
            _len += ncopy;
        }

        if (ncopy < len) {
            PacketHdr& hdr = *(PacketHdr* )(void *)_buf;
            uint16 ndata = std::min(len - ncopy, (size_t) hdr.pktLen);
            if (ndata + _len > sizeof(_buf)) {
                throw BadPacket();
            }

            memcpy(_buf + _len, buf + ncopy, ndata);
            ncopy += ndata;
            _len += ndata;
        }
        return ncopy;
    }

    bool verify() const {
        PacketHdr& hdr = *(PacketHdr* )(void *)_buf;
        return _NTOHS(hdr.magic) == PACKET_MAGIC;
    }

    bool is_complete() const {
        PacketHdr& hdr = *(PacketHdr* )(void *)_buf;
        return hdr.pktLen + Packet::kHdrLen == _len;
    }

    const IPacket& operator >> (int8& data) const {
        checkOverread(sizeof(data));
        data = *((int8* )&_buf[_pos]);
        _pos += sizeof(int8);
        assert(_pos < PACKET_SIZE);
        return *this;
    }

    const IPacket& operator >> (uint8& data) const {
        checkOverread(sizeof(data));
        data = *((uint8* )&_buf[_pos]);
        _pos += sizeof(uint8);
        assert(_pos < PACKET_SIZE - sizeof(PacketHdr));
        return *this;
    }

    const IPacket& operator >> (int16& data) const {
        checkOverread(sizeof(data));
        data = _NTOHS(*((int16* )&_buf[_pos]));
        _pos += sizeof(int16);
        assert(_pos < PACKET_SIZE - sizeof(PacketHdr));
        return *this;
    }

    const IPacket& operator >> (uint16& data) const {
        checkOverread(sizeof(data));
        data = _NTOHS(*((uint16* )&_buf[_pos]));
        _pos += sizeof(uint16);
        assert(_pos < PACKET_SIZE - sizeof(PacketHdr));
        return *this;
    }

    const IPacket& operator >> (int32& data) const {
        checkOverread(sizeof(data));
        data = _NTOHL(*((int32* )&_buf[_pos]));
        _pos += sizeof(int32);
        assert(_pos < PACKET_SIZE - sizeof(PacketHdr));
        return *this;
    }

    const IPacket& operator >> (uint32& data) const {
        checkOverread(sizeof(data));
        data = _NTOHL(*((uint32* )&_buf[_pos]));
        _pos += sizeof(uint32);
        assert(_pos < PACKET_SIZE - sizeof(PacketHdr));
        return *this;
    }

    const IPacket& operator >> (int64& data) const {
        checkOverread(sizeof(data));
        data = _NTOHLL(*((int64* )&_buf[_pos]));
        _pos += sizeof(int64);
        assert(_pos < PACKET_SIZE - sizeof(PacketHdr));
        return *this;
    }

    const IPacket& operator >> (uint64& data) const {
        checkOverread(sizeof(data));
        data = _NTOHLL(*((uint64* )&_buf[_pos]));
        _pos += sizeof(uint64);
        assert(_pos < PACKET_SIZE - sizeof(PacketHdr));
        return *this;
    }

    const IPacket& operator >> (char* str) const {
        uint16 len;
        checkOverread(sizeof(len));
        (*this ) >> len;

        checkOverread(len);
        memcpy(str, &_buf[_pos], len);
        _pos += len;
        assert(_pos < PACKET_SIZE - sizeof(PacketHdr));
        return *this;
    }

    const IPacket& operator >> (std::string& str) const {
        uint16 len;
        checkOverread(sizeof(len));
        (*this ) >> len;

        checkOverread(len);
        str = std::string(&_buf[_pos], len);
        _pos += len;
        assert(_pos < PACKET_SIZE);
        return *this;
    }

    void checkOverread(size_t rsize) const throw (PacketOverread) {
        if (_pos + rsize > _len) {
            throw PacketOverread();
        }
    }

    virtual void setData(const unsigned char* buf, uint16 len) {
        assert(len <= PACKET_SIZE);
        memcpy(_buf + Packet::kHdrLen, buf, len);
        _len = Packet::kHdrLen + len;
        _pos = Packet::kHdrLen; // read from here
    }

    void decode(const uint32* key);
};

class OPacket : public Packet {
public:
    OPacket() {
        PacketHdr& hdr = *(PacketHdr* )(void *)_buf;
        hdr.setMagic(PACKET_MAGIC);
        hdr.pktLen = 0;
        hdr.flag = 0;
        hdr.mid = 0;
        hdr.pid = 0;

        _pos = Packet::kHdrLen;
        _len = Packet::kHdrLen;
    }

    ~OPacket(void) {

    }

    uint16 getLen() const {
        return _len;
    }

    void adjust(size_t size) {
        _pos += size;
        if (_pos > _len) {
            _len = _pos;
        }
    }

    OPacket& operator << (int8 data) {
        checkOverwrite(sizeof(data));
        *((int8* )&_buf[_pos]) = data;
        adjust(sizeof(int8));
        return *this;
    }

    OPacket& operator << (uint8 data) {
        checkOverwrite(sizeof(data));
        *((uint8* )&_buf[_pos]) = data;
        adjust(sizeof(uint8));
        return *this;
    }

    OPacket& operator << (int16 data) {
        checkOverwrite(sizeof(data));
        *((int16* )&_buf[_pos]) = _HTONS(data);
        adjust(sizeof(int16));
        return *this;
    }

    OPacket& operator << (uint16 data) {
        checkOverwrite(sizeof(data));
        *((uint16* )&_buf[_pos]) = _HTONS(data);
        adjust(sizeof(uint16));
        return *this;
    }

    OPacket& operator << (int32 data) {
        checkOverwrite(sizeof(data));
        *((int32* )&_buf[_pos]) = _HTONL(data);
        adjust(sizeof(int32));
        return *this;
    }

    OPacket& operator << (uint32 data) {
        checkOverwrite(sizeof(data));
        *((uint32* )&_buf[_pos]) = _HTONL(data);
        adjust(sizeof(uint32));
        return *this;
    }

    OPacket& operator << (int64 data) {
        *((int64* )&_buf[_pos]) = _HTONLL(data);
        adjust(sizeof(int64));
        return *this;
    }

    OPacket& operator << (uint64 data) {
        checkOverwrite(sizeof(data));
        *((uint64* )&_buf[_pos]) = _HTONLL(data);
        adjust(sizeof(uint64));
        return *this;
    }

    OPacket& operator << (const char* str) {
        checkOverwrite(sizeof(uint16));
        uint16 len = (uint16 )strlen(str);
        checkOverwrite(len);
        *this << len;
        memcpy(&_buf[_pos], str, len);
        adjust(len);
        return *this;
    }

    OPacket& operator << (const std::string& str) {
        checkOverwrite(sizeof(uint16));
        uint16 len = (uint16 )str.size();
        (*this ) << len;
        checkOverwrite(len);

        memcpy(&_buf[_pos], str.c_str(), len);
        adjust(len);

        return *this;
    }

    virtual void setData(const unsigned char* buf, uint16 len) {
        assert(len <= PACKET_SIZE);
        memcpy(_buf + Packet::kHdrLen, buf, len);
        _len = Packet::kHdrLen + len;
        _pos = _len;  // always append data is overwritten
    }

    void checkOverwrite(size_t wsize) const throw (PacketOverwrite) {
        if (_len + wsize > PACKET_SIZE) {
            throw PacketOverwrite();
        }
    }

    void wind() {
        PacketHdr& hdr = *(PacketHdr* )(void *)_buf;
        hdr.setLen(getDataSize());
    }

    void unwind() {

    }

    void encode(const uint32* key);
};

} // namespace util

#endif /* ROBOT_RELEASE_PACKET_H */
