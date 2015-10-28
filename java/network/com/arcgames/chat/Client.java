package com.arcgames.chat;

import java.io.IOException;
import java.util.Random;

import com.arcgames.net.InputPacket;
import com.arcgames.net.OutputPacket;
import com.arcgames.net.ArcConnection;
import com.arcgames.net.Message;

class Client extends ArcConnection {
    private String userName;
    private String passWord;

    public Client(String userName, String passWord) {
        super();

        this.userName = userName;
        this.passWord = passWord;
    }

    public void login(String host, int port) throws IOException {
        super.connect(host, port);
    }

    @Override
    public void onConnected() {
        OutputPacket packet = new OutputPacket(Message.C2L_LOGIN);
        packet.putString(userName).putString(passWord).putString("en");
        packet.putString("arcversion:V99.99.99.9999 OS:archLinux").putShort((short) 0);
        try {
            sendPacket(packet);
        } catch (IOException e) {
            System.out.println(e);
        }
    }

    @Override
    public void dispatch(InputPacket packet) {
        switch (packet.getMsgId()) {
            case Message.L2C_LOGIN_RESULT:
                processLoginResult(packet);
                break;
            default:
                System.out.println("dispatch message id: " + packet.getMsgId());
                break;
        }
    }

    private void processLoginResult(InputPacket packet) {
        byte result = packet.next();
        byte errorCode = packet.next();
        System.out.println("Login result " + result + ", errorCode " + errorCode);
        String sessionId = packet.nextString();
        String nickname = packet.nextString();
        System.out.println("Session id " + sessionId + ", nickname " + nickname);
    }

    public static void main(String[] args) {
        String host = args.length > 0 ? args[0] : "10.253.89.156";
        int port = args.length > 1 ? Integer.parseInt(args[1]) : 9881;

        System.out.println("connecting to " + host + ":" + port);


        Client client;

        try {
            client = new Client("arcrobot_1", "123456");

            client.login(host, port);

            while (true) {
                client.recv();
            }
        } catch (IOException ioexception) {
            System.err.println("caught exception: " + ioexception.getMessage());

            for (StackTraceElement e : ioexception.getStackTrace()) {
                System.err.println(e);
            }
        }
    }
}
