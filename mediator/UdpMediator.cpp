#include"UdpMediator.h"
#include"UdpNet.h"
UdpMediator::UdpMediator()
{
	m_pNet = new UdpNet(this);
}
UdpMediator::~UdpMediator()//使用时，父类指针指向子类，使用虚析构
{

}
//初始化网络
bool UdpMediator::OpenNet()
{
	if (!m_pNet->InitNet())
	{
		return false;
	}
	return true;
}
//关闭网络
void UdpMediator::CloseNet()
{
	m_pNet->UnlnitNet();
}
//发送数据
bool UdpMediator::SendData(long lSendIp, const char* buf, int nLen)
{
	if (!m_pNet->SendData(lSendIp, buf, nLen))
	{
		return false;
	}
	return true;	
}
//处理数据
#include<iostream>
using namespace std;
void UdpMediator::DealData(long lSendIp, const char* buf, int nLen)
{
	cout << buf << endl;
}