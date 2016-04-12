from ouou import message

def parse_hex(hex_data):
    b = bytearray.fromhex(hex_data)
    print(message.Message.allfrombytes(b))

if __name__ == '__main__':
    import sys

    if len(sys.argv) > 1:
        parse_hex(sys.argv[1])
    else:
        # 5506
        parse_hex('7b8815820020000100020800484b7c3030373030122a2c2d2e2f303132333435363738393a3b3c7d')
        # 5512
        parse_hex('7b8815820020000100020800484b7c3030373030122a2c2d2e2f303132333435363738393a3b3c7d')
        # 5513
        parse_hex('7b8915820020000100010800484b7c30303730300e0102030405060708090a0b0c0d0eaa848f5f7d')
        # 5514
        parse_hex('7b8a1582001f000100010800484b7c303037303008010203040506070801a0860100102700007d')
        # 5515
        parse_hex('7b8b1582000e000100010800484b7c3030373030147d')
        # 5516
        parse_hex('7b8c15820036000100010d0100000a0008030405090a0b0c0d0103000800484b7c30303730300800484b7c30303030310b004e41534441517c4141504c7d')
        # 5518
        parse_hex('7b8e158200230001006161610000000000000000000000000000000000000000000000000000000000007d')
