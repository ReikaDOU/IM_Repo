#include"TcpServerMediator.h"
#include"TcpServer.h"
TcpServerMediator::TcpServerMediator()
{
	m_pNet = new TcpServer(this);
}
TcpServerMediator::~TcpServerMediator()
{
	if (m_pNet)
	{
		delete m_pNet;
		m_pNet = nullptr;
	}
}

bool TcpServerMediator::OpenNet()
{
	if (!m_pNet->InitNet())
	{
		return false;
	}
	return true;
}

void TcpServerMediator::CloseNet()
{
	m_pNet->UnInitNet();
}

bool TcpServerMediator::SendData(long lSendIp, const char* buf, int nLen)
{
	if (!m_pNet->SendData(lSendIp, buf, nLen))
	{
		return false;
	}
	return true;
}

void TcpServerMediator::DealData(long lSendIp, const char* buf, int nLen)
{
	cout << lSendIp << ":" << buf << endl;
	SendData(lSendIp, buf, nLen);
}
