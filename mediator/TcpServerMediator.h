#pragma once
#include"INetMediator.h"
#include<iostream>
#include"../Ckernel.h"
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
	bool SendData(long lSendIp, char* buf, int nLen);
	//��������
	void DealData(long lSendIp, char* buf, int nLen);
public:
	static CKernel* p_kernel;
};