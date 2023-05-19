#include"UdpMediator.h"
#include"UdpNet.h"
UdpMediator::UdpMediator()
{
	m_pNet = new UdpNet(this);
}
UdpMediator::~UdpMediator()//ʹ��ʱ������ָ��ָ�����࣬ʹ��������
{

}
//��ʼ������
bool UdpMediator::OpenNet()
{
	if (!m_pNet->InitNet())
	{
		return false;
	}
	return true;
}
//�ر�����
void UdpMediator::CloseNet()
{
	m_pNet->UnlnitNet();
}
//��������
bool UdpMediator::SendData(long lSendIp, const char* buf, int nLen)
{
	if (!m_pNet->SendData(lSendIp, buf, nLen))
	{
		return false;
	}
	return true;	
}
//��������
#include<iostream>
using namespace std;
void UdpMediator::DealData(long lSendIp, const char* buf, int nLen)
{
	cout << buf << endl;
}