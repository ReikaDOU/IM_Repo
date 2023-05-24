#include"TcpServerMediator.h"
#include"TcpServer.h"
#include"../CKernel.h"
TcpServerMediator::TcpServerMediator()
{
	m_pNet = new TcpServer(this);
}
TcpServerMediator::~TcpServerMediator()
{
	
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

bool TcpServerMediator::SendData(long lSendIp, char* buf, int nLen)
{
	if (!m_pNet->SendData(lSendIp, buf, nLen))
	{
		return false;
	}
	return true;
}

void TcpServerMediator::DealData(long lSendIp, char* buf, int nLen)
{
	cout << lSendIp << ":" << buf << endl;
	CKernel::p_kernel->DealData(lSendIp, buf, nLen);
}
