package com.arcgames.net;

import java.io.OutputStream;
import java.io.IOException;
import com.arcgames.util.Tea;

public class OutputPacket extends Packet {
    public OutputPacket(short msgId) {
        super();
        byteBuffer.putShort(Packet.MAGIC_OFFSET, Packet.PACKET_MAGIC);
        byteBuffer.putShort(Packet.FLAG_OFFSET, (short)0);
        byteBuffer.putShort(Packet.DATA_LEN_OFFSET, (short) 0);
        byteBuffer.putShort(Packet.MID_OFFSET, msgId);
        byteBuffer.putInt(Packet.PID_OFFSET, 0);
        byteBuffer.position(Packet.HEAD_SIZE);
    }

    public OutputPacket put(byte value) {
        byteBuffer.put(value);

        return this;
    }

    public OutputPacket put(byte[] value) {
        byteBuffer.put(value);

        return this;
    }

    public OutputPacket putShort(short value) {
        byteBuffer.putShort(value);

        return this;
    }

    public OutputPacket putInt(int value) {
        byteBuffer.putInt(value);

        return this;
    }

    public OutputPacket putLong(long value) {
        byteBuffer.putLong(value);

        return this;
    }

    public OutputPacket putString(String value) {
        byteBuffer.putShort((short) value.length());
        byteBuffer.put(value.getBytes());

        return this;
    }

    OutputPacket encrypt(byte[] secureKey) {
        short dataSize = getDataSize();

        if (dataSize != 0) {
            byte[] data = new byte[dataSize];
            byteBuffer.position(Packet.HEAD_SIZE);
            byteBuffer.get(data, 0, dataSize);
            byte[] encryptedData = new Tea(secureKey).encrypt(data);
            setData(encryptedData);
        }

        return this;
    }

    OutputPacket rewind() {
        setDataSize((short) (byteBuffer.position() - Packet.HEAD_SIZE));

        return this;
    }

    OutputPacket flush(OutputStream stream) throws IOException {
        stream.write(byteBuffer.array(), 0, getPacketSize());

        return this;
    }
}
