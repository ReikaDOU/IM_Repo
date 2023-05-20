#pragma once
#include"INetMediator.h"
#include<iostream>
using namespace std;
class TcpServerMediator :public INetMediator
{
public:
	TcpServerMediator();
	~TcpServerMediator();
   //初始化网络
	bool OpenNet();
	//关闭网络
	void CloseNet();
	//发送数据
	bool SendData(long lSendIp, const char* buf, int nLen);
	//处理数据
	void DealData(long lSendIp, const char* buf, int nLen);
};