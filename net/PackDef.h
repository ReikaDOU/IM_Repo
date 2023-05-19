#pragma once
#define DEF_UDP_PORT 12345
#define DEF_BUFFER_SZIE 4096
#define DEF_NAME_SIZE 60
#define DEF_CONTENT_SIZE 1024
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

//协议结构
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