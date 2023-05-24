#pragma once
#include"CMysql.h"
#include"INetMediator.h"
#include"PackDef.h"
#include<map>
#include<iostream>
using namespace std;

class CKernel;
//定义函数指针
typedef void (CKernel::* FUN)(long lSendIp, char* buf, int nLen);

class CKernel
{
public:
	CKernel(void);
	~CKernel(void);
	bool startServer();
	void closeServer();
	void DealData(long lSendIp, char* buf, int nLen);
	//处理登录请求函数
	void DealLoginRq(long lSendIp, char* buf, int nLen);
	//处理注册请求函数
	void DealRegisterRq(long lSendIp, char* buf, int nLen);
	//处理聊天请求
	void dealChatRq(long lSendIp, char* buf, int nLen);
private:
	//获取好友列表，包括自己
	void getUserList(int id);
	//根据id查询用户信息
	void getUserInfoFromSql(int id, STRU_FRIEND_INFO* info);
	void setProtocalMap();
private:
	CMySql* m_pMySql;
	INetMediator* m_pServer;
	//协议映射表
	FUN m_netProtocalMap[DEF_PROTOCAL_COUNT];
	//保存id和sock的映射关系
	map<int, int>m_mapIdToSock;


public:
	static CKernel* p_kernel;//使用静态，指针不依赖于对象存在
};