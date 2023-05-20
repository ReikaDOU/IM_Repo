#pragma once
#include<winsock2.h>
#include<ws2tcpip.h>
#include<windows.h>
#pragma comment(lib,"ws2_32.lib")
#include"PackDef.h"

class	 INetMediator;//������ͷ�ļ���ֱ��������
class INet
{
public:
	INet(){}
	virtual ~INet(){}//ʹ��ʱ������ָ��ָ�����࣬ʹ��������
	//��ʼ������
	virtual bool InitNet() = 0;
	//�ر�����
	virtual void UnInitNet() = 0;
	//��������
	virtual bool SendData(long lSendIp, const char* buf, int nLen) = 0;
protected:
	//��������
	virtual void RecvData() = 0;
	INetMediator* m_pMediator;//�н��߽ӿ�ָ�룬���ڽ��������������⴫��

};