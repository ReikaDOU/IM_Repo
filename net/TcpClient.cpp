#include"TcpClient.h"
#include"INetMediator.h"
#include<process.h>
#include<iostream>
TcpClient::TcpClient(INetMediator* pMediator):
	m_sock(INVALID_SOCKET), m_hThreadHandle(0), m_isStop(false)
{
	m_pMediator = pMediator;
}
TcpClient::~TcpClient()
{
	UnInitNet();
}
bool TcpClient::InitNet()
{
	//加载库
	WORD wVersionResquested = { 0 };
	WSADATA wsaData = { 0 };
	int result = 0;

	wVersionResquested = MAKEWORD(2, 2);
	result = WSAStartup(wVersionResquested, &wsaData);
	if (result != 0)
	{
		return false;
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		UnInitNet();
		return false;
	}
	//2·创建套接字
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//
	if (INVALID_SOCKET == m_sock)
	{
		UnInitNet();
		return false;
	}

	//3·选地址--绑定IP
	sockaddr_in server;
	int nlen = sizeof(server);
	server.sin_family = AF_INET;
	server.sin_port = htons(DEF_TCP_PORT);//
	inet_pton(AF_INET, DEF_SERVER_IP, &server.sin_addr);

	result = connect(m_sock, (SOCKADDR*)&server, nlen);//
	if (SOCKET_ERROR == result)
	{
		std::cout << WSAGetLastError() << std::endl;
		UnInitNet();
		return 1;
	}

	//接收数据--创建线程

	/* C/C++ RunTime库 strcpy 创建内存块 CreateThread创建的线程不会回收内存块，造成内存泄露
	ExitThread推出线程  _beginthreadex底层也是调用的CreateThread，退出时调用ExitThread，并且回收创建的内存块	*/

	m_hThreadHandle = (HANDLE)_beginthreadex(NULL, 0, &RecvThread, this, 0, NULL);

	return true;
}
void TcpClient::UnInitNet()
{
	//退出线程
	m_isStop = true;
	if (m_hThreadHandle)
	{
		if (WAIT_TIMEOUT == WaitForSingleObject(m_hThreadHandle, 100))
		{
			TerminateThread(m_hThreadHandle, -1);
		}
		CloseHandle(m_hThreadHandle);
		m_hThreadHandle = NULL;
	}

	//关闭socket
	if (m_sock && m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
	}

	//卸载库
	WSACleanup();
}
bool TcpClient::SendData(long lSendIp, char* buf, int nLen)
{
	//判断输入参数合法性
	if (!buf || nLen <= 0)
	{
		return false;
	}
	//防止粘包
	if (send(m_sock, (char*)&nLen, sizeof(int), 0) <= 0)
	{
		return false;
	}
	if (send(m_sock, buf, nLen, 0) <= 0)
	{
		return false;
	}

	return true;
}

unsigned int _stdcall TcpClient::RecvThread(void* lpVoid)
{
	TcpClient* pThis = (TcpClient*)lpVoid;
	pThis->RecvData();
	return 1;
}

void TcpClient::RecvData()
{
	int nPackSize = 0;//存储包大小
	int nRes = 0;
	int offset = 0;//偏移量
	while (!m_isStop)
	{
		//先接收包大小，再接收包
		nRes = recv(m_sock, (char*)&nPackSize, sizeof(int), 0);
		//接收缓冲区拷贝包大小
		if (nRes <= 0)
		{
			break;
		}
		char* buf = new char[nPackSize];
		while (nPackSize)
		{
			nRes = recv(m_sock, buf + offset, nPackSize, 0);
			if (nRes > 0)
			{
				nPackSize -= nRes;
				offset += nRes;
			}
		}
		this->m_pMediator->DealData(m_sock, buf, offset);
	}
}
