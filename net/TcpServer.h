#pragma once
#include"INet.h"
#include<list>
#include<map>
#include<iostream>
using namespace std;
class TcpServer :public INet
{
public:
	TcpServer(INetMediator* pMediator);
	~TcpServer();//复用，使用虚析构
	//初始化网络
	bool InitNet();
	//关闭网络
	void UnInitNet();
	//发送数据
	bool SendData(long lSendIp, const char* buf, int nLen);
protected:
	static unsigned int  _stdcall RecvThread(void* lpVoid);
	//接收数据
	void RecvData();
	SOCKET m_sock;
	bool m_isStop;

	list<HANDLE> m_hThreadHandleList;//管理线程
	static unsigned int _stdcall AcceptThread(void* lpVoid);//
	map<unsigned int, SOCKET>m_mapTHreadIDToSocket;//管理socket
};