#pragma once
#include"UdpNet.h"
#include<process.h>
#include"INetMediator.h"
UdpNet::UdpNet(INetMediator* pMediator):m_sock(INVALID_SOCKET),m_hTHreadHandle(0),m_isStop(false)
{
	m_pMediator = pMediator;
}
UdpNet::~UdpNet()//ʹ��ʱ������ָ��ָ�����࣬ʹ��������
{
	UnInitNet();
}
//��ʼ������
bool UdpNet::InitNet()
{
	WORD wVersionResquested = { 0 };
	WSADATA wsaData = { 0 };
	int result = 0;

	wVersionResquested = MAKEWORD(2, 2);
	result = WSAStartup(wVersionResquested, &wsaData);
	if (result != 0)
	{
		return false;
	}

	//2�������׽���
	m_sock= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == m_sock)
	{
		UnInitNet();
		return false;
	}

	//3�����ù㲥Ȩ��
	BOOL bval = TRUE;
	setsockopt(m_sock, SOL_SOCKET, SO_BROADCAST, (char*)&bval, sizeof(bval));

	//4��ѡ��ַ--��IP
	sockaddr_in service;
	int nlen = sizeof(service);
	service.sin_family = AF_INET;
	service.sin_port = htons(DEF_UDP_PORT);//htons������ת���������ֽ���
	service.sin_addr.S_un.S_addr = INADDR_ANY;
	result = bind(m_sock, (sockaddr*)&service, sizeof(service));
	if (SOCKET_ERROR == result)
	{
		UnInitNet();
		return 1;
	}

	//��������--�����߳�
	
	/* C/C++ RunTime�� strcpy �����ڴ�� CreateThread�������̲߳�������ڴ�飬����ڴ�й¶
	ExitThread�Ƴ��߳�  _beginthreadex�ײ�Ҳ�ǵ��õ�CreateThread���˳�ʱ����ExitThread�����һ��մ������ڴ��	*/
	
	m_hTHreadHandle = (HANDLE)_beginthreadex(NULL, 0, &RecvThread, this, 0, NULL);

	return true;
}

unsigned int _stdcall UdpNet::RecvThread(void* lpVoid)
{
	UdpNet* pThis = (UdpNet*)lpVoid;
	pThis->RecvData();
	return 1;
}
//�ر�����
void UdpNet::UnInitNet()
{
	//�˳��߳�
	m_isStop = true;
	if (m_hTHreadHandle)
	{
		if (WAIT_TIMEOUT == WaitForSingleObject(m_hTHreadHandle, 100))
		{
			TerminateThread(m_hTHreadHandle, -1);
		}
		CloseHandle(m_hTHreadHandle);
		m_hTHreadHandle = NULL;
	}
	//�ر�socket��ж�ؿ�
	if (m_sock && m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
	}
	WSACleanup();

}
//��������
bool UdpNet::SendData(long lSendIp, const char* buf, int nLen)
{
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(DEF_UDP_PORT);
	addr.sin_addr.S_un.S_addr = lSendIp;
	if (sendto(m_sock, buf, nLen, 0, (sockaddr*)&addr, sizeof(addr)) <= 0)
	{
		return false;
	}
	return true;
}
//��������
void UdpNet::RecvData()
{
	char recvBuf[DEF_BUFFER_SZIE] = "";
	SOCKADDR_IN addrClient;
	int nSize = sizeof(addrClient);
	int nRecvNum = 0;
	m_isStop = false;
	while (!m_isStop)
	{
		nRecvNum = recvfrom(m_sock, recvBuf, sizeof(recvBuf), 0, (sockaddr*)&addrClient, &nSize);
		if (nRecvNum > 0)//���͸��н�
		{
			char* packBuf = new char[nRecvNum];
			memcpy(packBuf, recvBuf, nRecvNum);
			if (m_pMediator)
			{
				m_pMediator->DealData(addrClient.sin_addr.S_un.S_addr, packBuf, sizeof(packBuf));
			}//��������ռ䣬delete[]  packBuf
		}
	}
}