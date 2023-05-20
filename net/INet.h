#pragma once
#include<winsock2.h>
#include<ws2tcpip.h>
#include<windows.h>
#pragma comment(lib,"ws2_32.lib")
#include"PackDef.h"

class	 INetMediator;//不包含头文件，直接声明类
class INet
{
public:
	INet(){}
	virtual ~INet(){}//使用时，父类指针指向子类，使用虚析构
	//初始化网络
	virtual bool InitNet() = 0;
	//关闭网络
	virtual void UnInitNet() = 0;
	//发送数据
	virtual bool SendData(long lSendIp, const char* buf, int nLen) = 0;
protected:
	//接收数据
	virtual void RecvData() = 0;
	INetMediator* m_pMediator;//中介者接口指针，用于将网络数据向类外传输

};