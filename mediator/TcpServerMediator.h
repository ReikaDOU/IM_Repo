#pragma once
#include"INetMediator.h"
#include<iostream>
using namespace std;
class TcpServerMediator :public INetMediator
{
public:
	TcpServerMediator();
	~TcpServerMediator();
   //��ʼ������
	bool OpenNet();
	//�ر�����
	void CloseNet();
	//��������
	bool SendData(long lSendIp, const char* buf, int nLen);
	//��������
	void DealData(long lSendIp, const char* buf, int nLen);
};