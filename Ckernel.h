#pragma once
#include"CMysql.h"
#include"INetMediator.h"
#include"PackDef.h"
#include<map>
#include<iostream>
using namespace std;

class CKernel;
//���庯��ָ��
typedef void (CKernel::* FUN)(long lSendIp, char* buf, int nLen);

class CKernel
{
public:
	CKernel(void);
	~CKernel(void);
	bool startServer();
	void closeServer();
	void DealData(long lSendIp, char* buf, int nLen);
	//�����¼������
	void DealLoginRq(long lSendIp, char* buf, int nLen);
	//����ע��������
	void DealRegisterRq(long lSendIp, char* buf, int nLen);
	//������������
	void dealChatRq(long lSendIp, char* buf, int nLen);
private:
	//��ȡ�����б������Լ�
	void getUserList(int id);
	//����id��ѯ�û���Ϣ
	void getUserInfoFromSql(int id, STRU_FRIEND_INFO* info);
	void setProtocalMap();
private:
	CMySql* m_pMySql;
	INetMediator* m_pServer;
	//Э��ӳ���
	FUN m_netProtocalMap[DEF_PROTOCAL_COUNT];
	//����id��sock��ӳ���ϵ
	map<int, int>m_mapIdToSock;


public:
	static CKernel* p_kernel;//ʹ�þ�̬��ָ�벻�����ڶ������
};