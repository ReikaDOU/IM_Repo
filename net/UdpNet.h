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
	void UnInitNet();
	//发送数据
	bool SendData(long lSendIp, char* buf, int nLen);
protected:
	static unsigned int  _stdcall RecvThread(void* lpVoid);
	//接收数据
	 void RecvData();
	 SOCKET m_sock;
	 HANDLE m_hThreadHandle;
	 bool m_isStop;

};