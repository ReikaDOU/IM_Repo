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
	~TcpServer();//���ã�ʹ��������
	//��ʼ������
	bool InitNet();
	//�ر�����
	void UnInitNet();
	//��������
	bool SendData(long lSendIp, const char* buf, int nLen);
protected:
	static unsigned int  _stdcall RecvThread(void* lpVoid);
	//��������
	void RecvData();
	SOCKET m_sock;
	bool m_isStop;

	list<HANDLE> m_hThreadHandleList;//�����߳�
	static unsigned int _stdcall AcceptThread(void* lpVoid);//
	map<unsigned int, SOCKET>m_mapTHreadIDToSocket;//����socket
};