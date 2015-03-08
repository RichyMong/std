#pragma once

#define	G2N_PROTO_VER				1

#define ENCIPHER_KEY_LEN		4

// Base Packet ID
enum {
	MSG_S2C_SECURITY_INIT = 0x01,
	MSG_S2C_INIT_FINAL = 0x02,		// ³õÊ¼»¯Íê³É
};

enum {
	MSG_C2S_SECURITY_INIT = 0x50,
};
// GameServer to CoreCenter
enum {

	MSG_G2N_BASE = 0x1000,
	MSG_G2N_INIT,
	MSG_G2N_VERIFY_USER,
};


// CoreCenter to GameServer
enum {

	MSG_N2G_BASE = 0x2000,
	MSG_N2G_NOTIF,
	MSG_N2G_VERIFY_USER_RESULT,
};


#define	C2L_PROTO_VER				1

// Client to LoginMgr Server
enum {
	MSG_C2L_BASE = 0x3000,
	MSG_C2L_LOGIN,
	MSG_C2L_GET_TOKEN,
	MSG_C2L_LOGOUT,//add by zsl
	MSG_C2L_KEEP_ALIVE,//zdd by zsl
	MSG_C2L_TRACE_LOG, //add for client trace log
    MSG_C2L_EXIT,   //exit client or logout to login
	MSG_C2L_CHECK_PIN,
	MSG_C2L_REQUEST_NEWPIN
};

// LoginMgr Server to Client
enum {
	MSG_L2C_BASE = 0x4000,
	MSG_L2C_LOGIN_RESULT,
	MSG_L2C_GET_TOKEN_RESULT,
	MSG_L2C_NOTIF,
	MSG_L2C_KICK,
};

// LoginMgr to CoreCenter
enum {

	MSG_L2N_BASE = 0x5000,
	MSG_L2N_GET_TOKEN,
	MSG_L2N_LOGIN,
	MSG_L2N_OFFLINE,
};

// CoreCenter to LoginMgr
enum {
	MSG_N2L_BASE = 0x6000,
	MSG_N2L_NOTIF,
	MSG_N2L_LOGIN,
	MSG_N2L_OFFLINE,
};

// Client to GameServer
enum {
	MSG_C2G_LOGIN = 0x7000,
};

// GameServer to Client
enum {
	MSG_G2C_LOGIN_RESULT = 0x8000,
	MSG_G2C_INIT,
};

// Client to Switchboard
enum {
	MSG_C2S_REQADDR = 0x9000,
};

// Switchboard to Client
enum {
	MSG_S2C_REQADDR_RESULT = 0x9100
};

// LoginMgr to Switchboard
enum {
	MSG_L2S_INIT = 0x9200,
	MSG_L2S_UPDATE,
};
