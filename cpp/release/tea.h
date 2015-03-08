#pragma once

#include "util.h"

namespace util {

class Tea {
public:
	Tea(const uint32* key);
	~Tea(void);

	size_t encode(const unsigned char* indata, size_t inlen,
                  unsigned char* outdata, size_t outlen);

	size_t decode(const unsigned char* indata, size_t inlen,
                  unsigned char* outdata, size_t outlen);

protected:
	void encipher(uint32 * const v, const uint32 * const k);
	void decipher(uint32 * const v, const uint32 * const k);

protected:
	uint32		key_[4]; // 128 bits
};

}
