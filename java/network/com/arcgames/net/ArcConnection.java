package com.arcgames.net;

import java.net.Socket;
import java.net.InetSocketAddress;
import java.util.Random;
import java.util.Arrays;
import java.io.IOException;
import java.io.ByteArrayInputStream;
import java.io.InputStreamReader;
import java.security.PublicKey;
import java.security.NoSuchAlgorithmException;
import java.security.InvalidKeyException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.BadPaddingException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import org.bouncycastle.openssl.jcajce.JcaPEMKeyConverter;
import org.bouncycastle.openssl.PEMParser;
import org.bouncycastle.asn1.x509.SubjectPublicKeyInfo;

public abstract class ArcConnection {
    private static int SECURE_KEY_LEN = 16;

    private Socket socket;
    private byte[] secureKey = new byte[SECURE_KEY_LEN];
    private InputPacket recvPacket = new InputPacket();
    private boolean secured = false;

    {
        new Random().nextBytes(secureKey);

        ByteBuffer bb = ByteBuffer.wrap(secureKey);
        bb.order(ByteOrder.LITTLE_ENDIAN);
    }

    public ArcConnection() {
        socket = new Socket();
    }

    public ArcConnection(String host, int port) throws IOException {
        socket = new Socket(host, port);
    }

    public void connect(String host, int port) throws IOException {
        if (socket.isConnected()) {
            throw new RuntimeException("Already connected to " +
                    socket.getInetAddress() + ":" + socket.getPort());
        }

        socket.connect(new InetSocketAddress(host, port));
    }

    public void recv() throws IOException {
        if (recvPacket.readFromStream(socket.getInputStream()) == false) {
            socket.close();
        } else if (recvPacket.isComplete()) {
            InputPacket result = recvPacket.decrypt(secureKey);

            int msgId = result.getMsgId();
            if (msgId == Message.S2C_SECURITY_INIT) {
                initSecurity(result.nextString());
            } else if (msgId == Message.S2C_INIT_FINAL) {
                System.out.println("complete security init");
                secured = true;
                onConnected();
            } else {
                dispatch(result);
            }

            recvPacket.reset();
        }
    }

    protected void sendPacket(OutputPacket outPacket) throws IOException {
        outPacket.rewind();

        if (secured) {
            outPacket.encrypt(secureKey);
        }

        outPacket.flush(socket.getOutputStream());
    }

    private void sendSecureKey(PublicKey pubKey) {
        try {
            Cipher cipher = Cipher.getInstance("RSA/ECB/OAEPWithSHA1AndMGF1Padding");
            cipher.init(Cipher.ENCRYPT_MODE, pubKey);
            byte[] cipherText = cipher.doFinal(secureKey);
            OutputPacket outPacket = new OutputPacket(Message.C2S_SECURITY_INIT);
            outPacket.put(cipherText);
            sendPacket(outPacket);
        } catch (NoSuchAlgorithmException | InvalidKeyException | IllegalBlockSizeException |
                 NoSuchPaddingException | BadPaddingException e) {
            throw new RuntimeException(e.getMessage());
        } catch (IOException e) {
            System.out.println(e);
        }
    }

    private void initSecurity(String key) {
        ByteArrayInputStream stream = new ByteArrayInputStream(key.getBytes());

        try {
            PEMParser pemRd = new PEMParser(new InputStreamReader(stream));
            SubjectPublicKeyInfo pub = SubjectPublicKeyInfo.getInstance(pemRd.readObject());
            JcaPEMKeyConverter converter = new JcaPEMKeyConverter();

            sendSecureKey(converter.getPublicKey(pub));
        } catch (IOException e) {
            throw new RuntimeException(e.getMessage());
        }
    }

    public abstract void dispatch(InputPacket packet);

    public abstract void onConnected();
}
