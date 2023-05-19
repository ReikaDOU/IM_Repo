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
	//���ؿ�
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
	//2�������׽���
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//
	if (INVALID_SOCKET == m_sock)
	{
		UnInitNet();
		return false;
	}

	//3��ѡ��ַ--��IP
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
	//����
	result = listen(m_sock, 10);
	if (SOCKET_ERROR == result)
	{
		UnInitNet();
		return false;
	}
	//�����߳�
	HANDLE hThreadHandle = (HANDLE)_beginthreadex(NULL, 0, &AcceptThread, this, 0, NULL);
	if (hThreadHandle)
	{
		m_hThreadHandleList.push_back(hThreadHandle);
	}
	return true;
}

void TcpServer::UnInitNet()
{
	//�˳��߳�
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

	//�ر�socket
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
	//ж�ؿ�
	WSACleanup();
}

bool TcpServer::SendData(long lSendIp, const char* buf, int nLen)
{
	//�ж���������Ϸ���
	if (!buf || nLen <= 0)
	{
		return false;
	}
	//��ֹճ��
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

unsigned int _stdcall TcpServer::RecvThread(void* lpVoid)
{
	TcpServer* pThis = (TcpServer*)lpVoid;
	pThis->RecvData();
	return 1;
}

void TcpServer::RecvData()
{
	//��ȡ�׽��֡��߳�ID
	Sleep(100);//�����̺߳�����ִ���̣߳��߳�id���׽�������map��ִ��ʱ��
	SOCKET sockWaiter = m_mapTHreadIDToSocket[GetCurrentThreadId()];

	if (sockWaiter || sockWaiter == INVALID_SOCKET)
	{
		return;
	}

	int nPackSize = 0;//�洢����С
	int nRes = 0;
	int offset = 0;//ƫ����
	while (!m_isStop)
	{
		//�Ƚ��հ���С���ٽ��հ�
		nRes = recv(m_sock, (char*)&nPackSize, sizeof(int), 0);
		//���ջ�������������С
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

unsigned int _stdcall TcpServer::AcceptThread(void* lpVoid)
{
	//����IOģ�͡��������Ӻ�ÿ���ͻ�����һ���߳�
	TcpServer* pThis = (TcpServer*)lpVoid;
	sockaddr_in addrClient;
	int nSize = sizeof(addrClient);
	while (!pThis->m_isStop)
	{
		//��������
		SOCKET sockWaiter = accept(pThis->m_sock, (sockaddr*)&addrClient, &nSize);
		cout << "IP: " << inet_ntoa(addrClient.sin_addr) << "connect" << endl;
		//�������տͻ������ݵ��߳�
		//ÿһ���ͻ��˶�Ӧһ���̡߳�һ���׽��֣���Ӧ��
		//�����߳�ʱ���߳�ID���׽��ְ��γ�ӳ��:map
		unsigned int threadID = 0;
		HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, RecvThread, pThis, 0, &threadID);
		//�����߳̾��
		pThis->m_hThreadHandleList.push_back(handle);
		//�����߳�ID��socket��ֵ��	
		pThis->m_mapTHreadIDToSocket[threadID] = sockWaiter;
	}
	return 0;
}
