#include"TcpServer.h"
#include"INetMediator.h"
#include<process.h>

TcpServer::TcpServer(INetMediator* pMediator) :
	m_sock(INVALID_SOCKET), m_isStop(false)
{
	m_pMediator = pMediator;
}
TcpServer::~TcpServer()
{
	UnInitNet();
}

bool TcpServer::InitNet()
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
	server.sin_addr.S_un.S_addr = INADDR_ANY;//
	result = bind(m_sock, (sockaddr*)&server, nlen);//
	if (SOCKET_ERROR == result)
	{
		UnInitNet();
		return 1;
	}
	//监听
	result = listen(m_sock, 10);
	if (SOCKET_ERROR == result)
	{
		UnInitNet();
		return false;
	}
	//创建线程
	HANDLE hThreadHandle = (HANDLE)_beginthreadex(NULL, 0, &AcceptThread, this, 0, NULL);
	if (hThreadHandle)
	{
		m_hThreadHandleList.push_back(hThreadHandle);
	}
	return true;
}

void TcpServer::UnInitNet()
{
	//退出线程
	m_isStop = true;
	for (auto ite = m_hThreadHandleList.begin(); ite != m_hThreadHandleList.end(); ite++)
	{
		if (*ite)
		{
			if (WAIT_TIMEOUT == WaitForSingleObject(*ite, 100))
			{
				TerminateThread(*ite, -1);
			}
			CloseHandle(*ite);
			*ite = NULL;
		}
	}

	//关闭socket
	if (m_sock && m_sock != INVALID_SOCKET)
	{
		closesocket(m_sock);
	}
	for (auto ite = m_mapTHreadIDToSocket.begin(); ite != m_mapTHreadIDToSocket.end(); ite++)
	{
		if (ite->second && ite->second != INVALID_SOCKET)
		{
			closesocket(ite->second);
		}
	}
	//卸载库
	WSACleanup();
}

bool TcpServer::SendData(long lSendIp, char* buf, int nLen)
{
	int len1 = nLen;
	//判断入参合法性
	if (!buf || nLen <= 0)
	{
		return false;
	}
	//发包大小
	if (send(lSendIp, (char*)&len1, sizeof(int), 0) <= 0)
	{
		return false;
	}
	//发包内容
	if (send(lSendIp, buf, nLen, 0) <= 0)
	{
		return false;
	}
	return true;
}

unsigned int _stdcall TcpServer::RecvThread(void* lpVoid)
{
	TcpServer* pThis = (TcpServer*)lpVoid;
	//pThis->RecvData();

	//获取套接字、线程ID
	Sleep(100);//创建线程后立刻执行线程，线程id与套接字载入map需执行时间
	SOCKET sockWaiter = pThis->m_mapTHreadIDToSocket[GetCurrentThreadId()];

	if (!sockWaiter || sockWaiter == INVALID_SOCKET)
	{
		return 1;
	}
	int nPackSize = 0;//存储包大小
	int nRes = 0;
	int offset = 0;//偏移量
	pThis->m_isStop = false;
	while (!pThis->m_isStop)
	{
		//先接收包大小，再接收包
		nRes = recv(sockWaiter, (char*)&nPackSize, sizeof(int), 0);
		//接收缓冲区拷贝包大小
		if (nRes <= 0)
		{
			break;
		}
		char* buf = new char[nPackSize];
		while (nPackSize)
		{
			nRes = recv(sockWaiter, buf + offset, nPackSize, 0);
			if (nRes > 0)
			{
				nPackSize -= nRes;
				offset += nRes;
			}
		}
		pThis->m_pMediator->DealData(sockWaiter, buf, offset);
	}
	
}

void TcpServer::RecvData()
{
	int nPackSize = 0;//存储包大小
	int nRes = 0;
	int offset = 0;//偏移量
	m_isStop = false;
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
		m_pMediator->DealData(m_sock, buf, offset);
	}
}
unsigned int _stdcall TcpServer::AcceptThread(void* lpVoid)
{
	//网络IO模型、接收连接后、每个客户端需一个线程
	TcpServer* pThis = (TcpServer*)lpVoid;
	sockaddr_in addrClient;
	int nSize = sizeof(addrClient);
	char aa[100];
	while (!pThis->m_isStop)
	{
		//接收连接
		SOCKET sockWaiter = accept(pThis->m_sock, (sockaddr*)&addrClient, &nSize);
		cout << "IP: " << inet_ntop(AF_INET,(SOCKADDR*)&addrClient.sin_addr,aa,100) << "connect" << endl;
		//创建接收客户端数据的线程
		//每一个客户端对应一个线程、一个套接字（对应）
		//创建线程时，线程ID与套接字绑定形成映射:map
		unsigned int threadID = 0;
		HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, RecvThread, pThis, 0, &threadID);
		//保存线程句柄
		pThis->m_hThreadHandleList.push_back(handle);
		//保存线程ID与socket键值对	
		pThis->m_mapTHreadIDToSocket[threadID] = sockWaiter;
	}
	return 0;
}
