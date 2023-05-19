#pragma once
#define DEF_UDP_PORT 12345
#define DEF_BUFFER_SZIE 4096
#define DEF_NAME_SIZE 60
#define DEF_CONTENT_SIZE 1024
//Э��ͷ
#define DEF_PROTOCAL_BASE 1000
//����֪ͨ���ظ�
#define DEF_PROTOCAL_ONLINE_RQ  (DEF_PROTOCAL_BASE+1)
#define DEF_PROTOCAL_ONLINE_RS  (DEF_PROTOCAL_BASE+2)
//����֪ͨ
#define DEF_PROTOCAL_OFFLINE (DEF_PROTOCAL_BASE+3)
//������Ϣ
#define DEF_PROTOCAL_DATEINFO_RQ (DEF_PROTOCAL_BASE+4)
#define DEF_PROTOCAL_DATEINFO_RS (DEF_PROTOCAL_BASE+5)

//Э��ṹ
//����֪ͨ���ظ�//Э��ͷ�����֡�id...
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
//����֪ͨ
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
//������Ϣ֪ͨ
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