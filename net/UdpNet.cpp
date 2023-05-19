#pragma once
#include"UdpNet.h"
#include<process.h>
#include"INetMediator.h"
UdpNet::UdpNet(INetMediator* pMediator):m_sock(INVALID_SOCKET),m_hTHreadHandle(0),m_isStop(false)
{
	m_pMediator = pMediator;
}
UdpNet::~UdpNet()//使用时，父类指针指向子类，使用虚析构
{
	UnInitNet();
}
//初始化网络
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

	//2·创建套接字
	m_sock= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == m_sock)
	{
		UnInitNet();
		return false;
	}

	//3·设置广播权限
	BOOL bval = TRUE;
	setsockopt(m_sock, SOL_SOCKET, SO_BROADCAST, (char*)&bval, sizeof(bval));

	//4·选地址--绑定IP
	sockaddr_in service;
	int nlen = sizeof(service);
	service.sin_family = AF_INET;
	service.sin_port = htons(DEF_UDP_PORT);//htons将整数转换成网络字节序
	service.sin_addr.S_un.S_addr = INADDR_ANY;
	result = bind(m_sock, (sockaddr*)&service, sizeof(service));
	if (SOCKET_ERROR == result)
	{
		UnInitNet();
		return 1;
	}

	//接收数据--创建线程
	
	/* C/C++ RunTime库 strcpy 创建内存块 CreateThread创建的线程不会回收内存块，造成内存泄露
	ExitThread推出线程  _beginthreadex底层也是调用的CreateThread，退出时调用ExitThread，并且回收创建的内存块	*/
	
	m_hTHreadHandle = (HANDLE)_beginthreadex(NULL, 0, &RecvThread, this, 0, NULL);

	return true;
}

unsigned int _stdcall UdpNet::RecvThread(void* lpVoid)
{
	UdpNet* pThis = (UdpNet*)lpVoid;
	pThis->RecvData();
	return 1;
}
//关闭网络
void UdpNet::UnInitNet()
{
	//退出线程
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
	//关闭socket，卸载库
	if (m_sock && m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
	}
	WSACleanup();

}
//发送数据
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
//接收数据
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
		if (nRecvNum > 0)//发送给中介
		{
			char* packBuf = new char[nRecvNum];
			memcpy(packBuf, recvBuf, nRecvNum);
			if (m_pMediator)
			{
				m_pMediator->DealData(addrClient.sin_addr.S_un.S_addr, packBuf, sizeof(packBuf));
			}//回收这个空间，delete[]  packBuf
		}
	}
}