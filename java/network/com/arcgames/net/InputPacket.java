package com.arcgames.net;

import java.util.Arrays;
import java.io.InputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import com.arcgames.util.Tea;

public class InputPacket extends Packet {
    private int wpos = 0;

    public InputPacket() {
        super();
    }

    public InputPacket(ByteBuffer buffer) {
        super(buffer);
    }

    public byte next() {
        return byteBuffer.get();
    }

    public short nextShort() {
        return byteBuffer.getShort();
    }

    public int nextInt() {
        return byteBuffer.getInt();
    }

    public String nextString() {
        short len = nextShort();
        byte[] bytes = new byte[len];
        byteBuffer.get(bytes);
        return new String(bytes);
    }

    public boolean isComplete() {
        return wpos >= getDataSize() + Packet.HEAD_SIZE;
    }

    boolean readFromStream(InputStream stream) {
        int readLen = 0;

        try {
            readLen = stream.read(byteBuffer.array(), wpos, MAX_SIZE - wpos);
        } catch(IOException exception) {
            // write this exception to the log file
        }

        if (readLen > 0) {
            wpos += readLen;
            if (isComplete()) {
                verify();
            }
        }

        return readLen > 0;
    }

    InputPacket decrypt(byte[] secureKey) {
        short dataSize = getDataSize();
        if (dataSize == 0 || isEncrypted() == false)
            return this;

        byte[] encryptedData = new byte[dataSize];
        byteBuffer.position(Packet.HEAD_SIZE);
        byteBuffer.get(encryptedData, 0, dataSize);

        Tea tea = new Tea(secureKey);

        byte[] originData = tea.decrypt(encryptedData);

        ByteBuffer result = ByteBuffer.allocate(Packet.HEAD_SIZE + originData.length);
        result.order(ByteOrder.LITTLE_ENDIAN);
        result.put(byteBuffer.array(), 0, Packet.HEAD_SIZE);
        result.put(originData, 0, originData.length);
        result.putShort(DATA_LEN_OFFSET, (short) originData.length);

        return new InputPacket(result);
    }

    public void reset() {
        // If there's still data in the buffer, we make a new buffer. This
        // may happen rarely.
        if (wpos > byteBuffer.position()) {
            int offset = getPacketSize();
            byteBuffer = ByteBuffer.wrap(byteBuffer.array(), offset, wpos - offset);
            wpos -= offset;
        } else {
            wpos = 0;
        }

        byteBuffer.position(Packet.HEAD_SIZE);
    }

    private void verify() {
        if (getMagic() != Packet.PACKET_MAGIC) {
            throw new RuntimeException("invalid packet magic");
        }

        short dataSize = getDataSize();
        if ((dataSize + Packet.HEAD_SIZE) > Packet.MAX_SIZE) {
            throw new RuntimeException("data size is too large: " + dataSize);
        }
    }
}
