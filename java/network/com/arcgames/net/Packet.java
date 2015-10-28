package com.arcgames.net;

import java.util.Arrays;
import java.io.InputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import com.arcgames.util.Tea;

class Packet {
    public final static int MAX_SIZE = 65536;
    public final static int HEAD_SIZE = 12;
    public final static int MAGIC_OFFSET = 0;
    public final static int FLAG_OFFSET = 2;
    public final static int DATA_LEN_OFFSET = 4;
    public final static int MID_OFFSET = 6;
    public final static int PID_OFFSET = 8;
    public final static int ENCRYPT_FLAG = 0x0004;
    public final static short PACKET_MAGIC = 0x5050;

    protected ByteBuffer byteBuffer;

    Packet() {
         byteBuffer = ByteBuffer.allocate(MAX_SIZE);
         byteBuffer.order(ByteOrder.LITTLE_ENDIAN);
         byteBuffer.position(Packet.HEAD_SIZE);
    }

    Packet(ByteBuffer buffer) {
         byteBuffer = buffer;
         byteBuffer.position(Packet.HEAD_SIZE);
    }

    public short getMsgId() {
        return byteBuffer.getShort(MID_OFFSET);
    }

    protected int getPacketSize() {
        return Packet.HEAD_SIZE + getDataSize();
    }

    public byte[] getData() {
        byte[] data = new byte[getDataSize()];
        System.arraycopy(byteBuffer.array(), HEAD_SIZE, data, 0, data.length);

        return data;
    }

    public Packet setData(byte[] data) {
        byteBuffer.position(Packet.HEAD_SIZE);
        byteBuffer.put(data);
        setDataSize(data.length);
        return this;
    }

    protected short getMagic() {
        return byteBuffer.getShort(MAGIC_OFFSET);
    }

    protected Packet setMagic() {
        byteBuffer.putShort(MAGIC_OFFSET, PACKET_MAGIC);
        return this;
    }

    protected short getFlag() {
        return byteBuffer.getShort(FLAG_OFFSET);
    }

    protected Packet setFlag(int flag) {
        byteBuffer.putShort(FLAG_OFFSET, (short) (getFlag() | flag));
        return this;
    }

    protected short getDataSize() {
        return byteBuffer.getShort(DATA_LEN_OFFSET);
    }

    protected Packet setDataSize(int dataSize) {
        byteBuffer.putShort(DATA_LEN_OFFSET, (short) dataSize);
        return this;
    }

    protected boolean isEncrypted() {
        return (getFlag() & ENCRYPT_FLAG) == ENCRYPT_FLAG;
    }
}
