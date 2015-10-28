package com.arcgames.util;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class Tea {
    private final static int SUGAR = 0x9E3779B9;
    private final static int CUPS  = 16;
    private final static int UNSUGAR = 0xE3779B90;
    private final static int KEY_SIZE = 4;

    private int[] S = new int[KEY_SIZE];

    /**
     * Initialize the cipher for encryption or decryption.
     * @param key a 16 byte (128-bit) key
     */
    public Tea(byte[] key) {
        if (key == null)
            throw new RuntimeException("Invalid key: Key was null");

        if (key.length < 16)
            throw new RuntimeException("Invalid key: Length was less than 16 bytes");

        for (int off = 0, i = 0; i < S.length; i++) {
            S[i] = ((key[off++] & 0xff)) |
                    ((key[off++] & 0xff) << 8) |
                    ((key[off++] & 0xff) << 16) |
                    ((key[off++] & 0xff) << 24);
        }
    }

    /**
     * Encrypt an array of bytes.
     * @param data the date to be encrypted
     * @return the encrypted data
     */
    public byte[] encrypt(byte[] data) {
        int paddedSize = (data.length + 7) & -8;

        byte[] encrypted = new byte[paddedSize + 2];
        encrypted[0] = (byte) ((data.length >> 8) & 0xff);
        encrypted[1] = (byte) (data.length & 0xff);

        int[] buffer = new int[paddedSize / 4];
        pack(data, buffer, 0);
        encipher(buffer);
        unpack(buffer, encrypted, 2);

        return encrypted;
    }

    /**
     * Decrypt an array of bytes.
     * @param ciper the cipher text to decrypt
     * @return the decrypted text
     */
    public byte[] decrypt(byte[] crypt) {
        assert (crypt.length - 2) % 8 == 0;

        int dataSize = ((crypt[0] & 0xff) << 8) | (crypt[1] & 0xff);

        int[] buffer = new int[(crypt.length - 2) / 4];
        pack(crypt, buffer, 2);
        decipher(buffer);

        byte[] data = new byte[dataSize];
        unpack(buffer, data, 0);

        return data;
    }

    private void encipher(int[] buf) {
        assert buf.length % 2 == 0;

        int v0, v1, sum, n;

        for (int i = 0; i < buf.length; i += 2) {
            n = CUPS;
            v0 = buf[i];
            v1 = buf[i + 1];
            sum = 0;
            while (n-->0) {
                sum += SUGAR;
                v0  += ((v1 << 4) + S[0]) ^ (v1 + sum) ^ ((v1 >>> 5) + S[1]);
                v1  += ((v0 << 4) + S[2]) ^ (v0 + sum) ^ ((v0 >>> 5) + S[3]);
            }
            buf[i] = v0;
            buf[i+1] = v1;
        }
    }

    private void decipher(int[] buf) {
        assert buf.length % 2 == 0;

        int n, v0, v1, sum;

        for (int i = 0; i < buf.length; i += 2) {
            n = CUPS;
            v0 = buf[i];
            v1 = buf[i+1];
            sum = UNSUGAR;

            while (n--> 0) {
                v1  -= ((v0 << 4) + S[2]) ^ (v0 + sum) ^ ((v0 >>> 5) + S[3]);
                v0  -= ((v1 << 4) + S[0]) ^ (v1 + sum) ^ ((v1 >>> 5) + S[1]);
                sum -= SUGAR;
            }

            buf[i] = v0;
            buf[i+1] = v1;
        }
    }

    private void pack(byte[] src, int[] dest, int srcOffset) {
        assert (src.length - srcOffset) / 4 <= dest.length;

        int shift = 0, j = 0;

        dest[j] = 0;

        for (int i = srcOffset; i < src.length; ++i) {
            dest[j] |= (src[i] & 0xff) << shift;
            if (shift == 24) {
                shift = 0;
                if (++j < dest.length) dest[j] = 0;
            } else {
                shift += 8;
            }
        }
    }

    private void unpack(int[] src, byte[] dest, int destOffset) {
        assert destOffset + src.length * 4 <= dest.length;

        int shift = 0;
        int j = 0;

        for (int i = destOffset; i < dest.length; i++) {
            dest[i] = (byte) ((src[j] >> shift) & 0xff);
            if (shift == 24) {
                shift = 0;
                j++;
            } else {
                shift += 8;
            }
        }

        return;
    }
}
