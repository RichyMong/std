from ctypes import *
import struct

def encipher(v, k):
    a = struct.unpack_from('I', k, 0)[0]
    b = struct.unpack_from('I', k, 4)[0]
    c = struct.unpack_from('I', k, 8)[0]
    d = struct.unpack_from('I', k, 12)[0]
    y = c_uint32(struct.unpack_from('I', v, 0)[0])
    z = c_uint32(struct.unpack_from('I', v, 4)[0])
    sum = c_uint32(0)
    delta = 0x9E3779B9
    n = 16
    w = [0,0]

    while (n > 0):
        sum.value += delta
        y.value += (z.value << 4) + a ^ z.value + sum.value ^ (z.value >> 5) + b
        z.value += (y.value << 4) + c ^ y.value + sum.value ^ (y.value >> 5) + d
        n -= 1

    w[0] = y.value
    w[1] = z.value
    return w

def decipher(v, k):
    a = struct.unpack_from('I', k, 0)[0]
    b = struct.unpack_from('I', k, 4)[0]
    c = struct.unpack_from('I', k, 8)[0]
    d = struct.unpack_from('I', k, 12)[0]
    y = c_uint32(struct.unpack_from('I', v, 0)[0])
    z = c_uint32(struct.unpack_from('I', v, 4)[0])
    sum = c_uint32(0xE3779b90)
    delta = 0x9E3779B9
    n = 16
    w = [0,0]

    while(n>0):
        z.value -= ( y.value << 4 ) + c ^ y.value + sum.value ^ ( y.value >> 5 ) + d
        y.value -= ( z.value << 4 ) + a ^ z.value + sum.value ^ ( z.value >> 5 ) + b
        sum.value -= delta
        n -= 1

    w[0] = y.value
    w[1] = z.value

    return w

CRYPTO_GROUP_SIZE = 8

def encode(v, k):
    inlen = len(v)
    groups = (inlen + (CRYPTO_GROUP_SIZE - 1)) / CRYPTO_GROUP_SIZE
    needlen = groups * CRYPTO_GROUP_SIZE + 2

    outdata = bytearray(needlen)
    outdata[0] = (inlen >> 8) & 0xFF
    outdata[1] = inlen & 0xFF
    outdata[2 : inlen + 2] = v
    for i in xrange(inlen + 2, needlen):
        outdata[i] = 0

    for i in xrange(2, needlen, CRYPTO_GROUP_SIZE):
        w = outdata[i : i + CRYPTO_GROUP_SIZE]
        struct.pack_into('II', outdata, i, *encipher(w, k))
    return outdata

def decode(v, k):
    inlen = len(v)
    needlen = (v[0] << 8) | v[1]

    if ((inlen - 2) % CRYPTO_GROUP_SIZE) != 0:
        print('decode: invalid size {}'.format(inlen))

    outdata = bytearray(needlen)
    outdata[0:] = v[2:]
    for i in xrange(0, inlen - 2, CRYPTO_GROUP_SIZE):
        w = outdata[i : i + CRYPTO_GROUP_SIZE]
        struct.pack_into('II', outdata, i, *decipher(w, k))
    return outdata
