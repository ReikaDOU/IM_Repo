#pragma once
#define DEF_UDP_PORT 12345
#define DEF_TCP_PORT (54320)
#define DEF_SERVER_IP ("10.56.61.135")
#define DEF_BUFFER_SZIE 4096
#define DEF_NAME_SIZE 60
#define DEF_CONTENT_SIZE 1024
#define DEF_MAX_SIZE 100

typedef int PackType;
//协议头
#define DEF_PROTOCAL_BASE 1000
//上线通知，回复
#define DEF_PROTOCAL_ONLINE_RQ  (DEF_PROTOCAL_BASE+1)
#define DEF_PROTOCAL_ONLINE_RS  (DEF_PROTOCAL_BASE+2)
//下线通知
#define DEF_PROTOCAL_OFFLINE (DEF_PROTOCAL_BASE+3)
//聊天消息
#define DEF_PROTOCAL_DATEINFO_RQ (DEF_PROTOCAL_BASE+4)
#define DEF_PROTOCAL_DATEINFO_RS (DEF_PROTOCAL_BASE+5)

//TCP--------------------------------

//TCP协议头---------------
//注册
#define DEF_PACK_REGISTER_RQ (DEF_PROTOCAL_BASE+10)
#define DEF_PACK_REGISTER_RS (DEF_PROTOCAL_BASE+11)
//登录
#define DEF_PACK_LOGIN_RQ (DEF_PROTOCAL_BASE+12)
#define DEF_PACK_LOGIN_RS (DEF_PROTOCAL_BASE+13)
//好友信息
#define DEF_PACK_FRIEND_INFO (DEF_PROTOCAL_BASE+14)
//添加好友
#define DEF_PACK_ADDFRIEND_RQ (DEF_PROTOCAL_BASE+15)
#define DEF_PACK_ADDFRIEND_RS (DEF_PROTOCAL_BASE+16)
//聊天
#define DEF_PACK_CHAT_RQ (DEF_PROTOCAL_BASE+17)
#define DEF_PACK_CHAT_RS (DEF_PROTOCAL_BASE+18)
//离线
#define DEF_PACK_OFFLINE_RQ (DEF_PROTOCAL_BASE+19)
//TCP请求返回结果------------
//注册返回结果
#define user_is_exist		(0)
#define register_success	(1)
//登录返回结果
#define user_not_exist		(0)
#define password_error		(1)
#define login_success		(2)

// 添加好友请求结果
#define no_this_user		(0)
#define user_refuse			(1)
#define user_offline		(2)
#define add_success			(3)

// 好友状态
#define status_online		(0)
#define status_offline		(1)

// 聊天信息发送结果
#define send_success		(0)
#define send_error			(1)


//协议结构-----------------------------
//注册
typedef struct STRU_REGISTER_RQ
{
	STRU_REGISTER_RQ() :type(DEF_PACK_REGISTER_RQ)
	{
		memset(tel, 0, DEF_MAX_SIZE);
		memset(name, 0, DEF_MAX_SIZE);
		memset(password, 0, DEF_MAX_SIZE);
	}
	PackType type;
	char tel[DEF_MAX_SIZE];
	char name[DEF_MAX_SIZE];
	char password[DEF_MAX_SIZE];
}STRU_REGISTER_RQ;
typedef struct STRU_REGISTER_RS
{
	STRU_REGISTER_RS() :type(DEF_PACK_REGISTER_RS),result(register_success)
	{
		
	}
	PackType type;
	int result;
}STRU_REGISTER_RS;
//登录
typedef struct STRU_LOGIN_RQ
{
	STRU_LOGIN_RQ() :type(DEF_PACK_LOGIN_RQ)
	{
		memset(tel, 0, DEF_MAX_SIZE);
		memset(password, 0, DEF_MAX_SIZE);
	}
	PackType type;
	char tel[DEF_MAX_SIZE];
	char password[DEF_MAX_SIZE];
}STRU_LOGIN_RQ;
typedef struct STRU_LOGIN_RS
{
	STRU_LOGIN_RS() :type(DEF_PACK_LOGIN_RS), result(register_success),userid(0)
	{

	}
	PackType type;
	int result;
	int userid;
}STRU_LOGIN_RS;
//好友信息：
typedef struct STRU_FRIEND_INFO
{
	STRU_FRIEND_INFO() :type(DEF_PACK_FRIEND_INFO),
		userid(0),iconid(0),state(status_online)
	{
		memset(name, 0, DEF_MAX_SIZE);
		memset(feeling, 0, DEF_MAX_SIZE);
	}
	PackType type;
	int userid;
	int iconid;
	int state;
	char name[DEF_MAX_SIZE];
	char feeling[DEF_MAX_SIZE];
}STRU_FRIEND_INFO;
//添加好友
typedef struct STRU_ADD_FRIEND_RQ
{
	STRU_ADD_FRIEND_RQ() :type(DEF_PACK_ADDFRIEND_RQ),userid(0)
	{
		memset(userName, 0, DEF_MAX_SIZE);
		memset(friendName, 0, DEF_MAX_SIZE);
	}
	PackType type;
	int userid;
	char userName[DEF_MAX_SIZE];
	char friendName[DEF_MAX_SIZE];
}STRU_ADD_FRIEND_RQ;
typedef struct STRU_ADD_FRIEND_RS
{
	STRU_ADD_FRIEND_RS() :type(DEF_PACK_ADDFRIEND_RQ), 
		userid(0),friendid(0),result(add_success)
	{
		memset(friendName, 0, DEF_MAX_SIZE);
	}
	PackType type;
	int userid;
	int friendid;
	int result;
	char friendName[DEF_MAX_SIZE];
}STRU_ADD_FRIEND_RS;
//聊天
typedef struct STRU_CHAT_RQ
{
	STRU_CHAT_RQ() :type(DEF_PACK_CHAT_RQ),	userid(0), friendid(0)
	{
		memset(content, 0, DEF_MAX_SIZE);
	}
	PackType type;
	int userid;
	int friendid;
	char content[DEF_MAX_SIZE];
}STRU_CHAT_RQ;
typedef struct STRU_CHAT_RS
{
	STRU_CHAT_RS() :type(DEF_PACK_CHAT_RS),
		userid(0), friendid(0), result(send_success)
	{
		
	}
	PackType type;
	int userid;
	int friendid;
	int result;
}STRU_CHAT_RS;

//------------------------------------------------------------------
//上线通知，回复//协议头、名字、id...
struct STRU_ONLINE_RS
{
	STRU_ONLINE_RS() :nType(DEF_PROTOCAL_ONLINE_RS)
	{
		memset(szName, 0, DEF_NAME_SIZE);
	}
	int nType;
	//long ip
	char szName[DEF_NAME_SIZE];
};
struct STRU_ONLINE_RQ
{
	STRU_ONLINE_RQ() :nType(DEF_PROTOCAL_ONLINE_RQ)
	{
		memset(szName, 0, DEF_NAME_SIZE);
	}
	int nType;
	//long ip
	char szName[DEF_NAME_SIZE];
};
//下线通知
struct STRU_OUTLINE
{
	STRU_OUTLINE() :nType(DEF_PROTOCAL_OFFLINE)
	{
		memset(szName, 0, DEF_NAME_SIZE);
	}
	int nType;
	//long ip
	char szName[DEF_NAME_SIZE];
};
//聊天信息通知
struct STRU_MESSAGE_RS
{
	STRU_MESSAGE_RS() :nType(DEF_PROTOCAL_DATEINFO_RS)
	{
		memset(szName, 0, DEF_NAME_SIZE);
	}
	int nType;
	//long ip
	char szName[DEF_NAME_SIZE];
};
struct STRU_MESSAGE_RQ
{
	STRU_MESSAGE_RQ() :nType(DEF_PROTOCAL_DATEINFO_RQ)
	{
		memset(szName, 0, DEF_NAME_SIZE);
	}
	int nType;
	//long ip
	char szName[DEF_NAME_SIZE];
};