#pragma once
#include"INetMediator.h"
#include<iostream>
using namespace std;
class TcpClientMediator :public INetMediator
{
public:
	TcpClientMediator();
	~TcpClientMediator();
	//��ʼ������
	bool OpenNet();
	//�ر�����
	void CloseNet();
	//��������
	bool SendData(long lSendIp, char* buf, int nLen);
	//��������
	void DealData(long lSendIp, char* buf, int nLen);
};