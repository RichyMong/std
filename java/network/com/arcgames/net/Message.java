package com.arcgames.net;

public class Message {
    public final static short S2C_SECURITY_INIT       = 0x0001;
    public final static short S2C_INIT_FINAL = 0x0002;
    public final static short C2S_SECURITY_INIT = 0x0050;

    public final static short C2L_BASE = 0x3000;
    public final static short C2L_LOGIN = C2L_BASE + 1;

    public final static short L2C_BASE = 0x4000;
    public final static short L2C_LOGIN_RESULT = L2C_BASE + 1;
}
