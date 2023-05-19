#pragma once
#include"INet.h"
class TcpClient :public INet
{
public:
	TcpClient(INetMediator* pMediator);
	~TcpClient();//���ã�ʹ��������
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
	HANDLE m_hTHreadHandle;
	bool m_isStop;
};