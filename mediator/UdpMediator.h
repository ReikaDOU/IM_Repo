#pragma once
#include"INetMediator.h"
class UdpMediator :public INetMediator
{
public:
	UdpMediator();
	 ~UdpMediator();//使用时，父类指针指向子类，使用虚析构
	//初始化网络
	 bool OpenNet() ;
	//关闭网络
	 void CloseNet() ;
	//发送数据
	 bool SendData(long lSendIp, const char* buf, int nLen);
	//处理数据
	 void DealData(long lSendIp, const char* buf, int nLen) ;
};
