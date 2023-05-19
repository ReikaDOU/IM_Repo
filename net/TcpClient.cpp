#include"TcpClient.h"
#include"INetMediator.h"
#include<process.h>
TcpClient::TcpClient(INetMediator* pMediator):
	m_sock(INVALID_SOCKET), m_hTHreadHandle(0), m_isStop(false)
{
	m_pMediator = pMediator;
}
TcpClient::~TcpClient()
{
	UnInitNet();
}
bool TcpClient::InitNet()
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
	server.sin_addr.S_un.S_addr = inet_addr(DEF_SERVER_IP);//
	result = connect(m_sock, (sockaddr*)&server, nlen);//
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
bool TcpClient::SendData(long lSendIp, const char* buf, int nLen)
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

unsigned int _stdcall TcpClient::RecvThread(void* lpVoid)
{
	TcpClient* pThis = (TcpClient*)lpVoid;
	pThis->RecvData();
	return 1;
}

void TcpClient::RecvData()
{
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
