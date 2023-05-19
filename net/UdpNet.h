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
	void UnlnitNet();
	//��������
	bool SendData(long lSendlp, const char* buf, int nLen);
protected:
	static unsigned int  _stdcall RecvThread(void* lpVoid);
	//��������
	 void RecvData();
	 SOCKET m_sock;
	 HANDLE m_hTHreadHandle;
	 bool m_isStop;

};