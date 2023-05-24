#pragma once
#include"INet.h"

class UdpNet:public INet
{
public:
	UdpNet(INetMediator* pMediator);
	~UdpNet();//���ã�ʹ��������
	//��ʼ������
	bool InitNet();
	//�ر�����
	void UnInitNet();
	//��������
	bool SendData(long lSendIp, char* buf, int nLen);
protected:
	static unsigned int  _stdcall RecvThread(void* lpVoid);
	//��������
	 void RecvData();
	 SOCKET m_sock;
	 HANDLE m_hThreadHandle;
	 bool m_isStop;

};