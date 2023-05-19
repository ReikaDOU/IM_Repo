#pragma once
#include"INet.h"

class UdpNet:public INet
{
public:
	UdpNet(INetMediator* pMediator);
	~UdpNet();//复用，使用虚析构
	//初始化网络
	bool InitNet();
	//关闭网络
	void UnlnitNet();
	//发送数据
	bool SendData(long lSendlp, const char* buf, int nLen);
protected:
	static unsigned int  _stdcall RecvThread(void* lpVoid);
	//接收数据
	 void RecvData();
	 SOCKET m_sock;
	 HANDLE m_hTHreadHandle;
	 bool m_isStop;

};