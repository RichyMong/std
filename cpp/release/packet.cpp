#include "packet.h"
#include "tea.h"

namespace util {

void OPacket::encode(const uint32* encrypt_key) {
	if (getFlag() & PACKET_FLAG_ENCRYPT) {
        return;
    }

	if (getDataSize() == 0) {
        setFlag(getFlag() | PACKET_FLAG_ENCRYPT);
        return;
	}

    Tea tea(encrypt_key);

	unsigned char buf[PACKET_SIZE];

	size_t len = tea.encode((unsigned char* )getData(), getDataSize(), buf, sizeof(buf));
	if (len <= 0) {
        assert(false);
        return;
	}

	setData(buf, (uint16 )len);
	setFlag(getFlag() | PACKET_FLAG_ENCRYPT);
}

void IPacket::decode(const uint32* decrypt_key) {
	if (getDataSize() == 0)
        return;

    Tea tea(decrypt_key);
	unsigned char buf[PACKET_SIZE];

	size_t len = tea.decode((unsigned char* )getData(), getDataSize(), buf, sizeof(buf));
	if (len == 0) {
        assert(false);
        return;
	}

	setData(buf, static_cast<uint16>(len));
}


} // namespace util {
