#pragma once
class INet;//不包含头文件，直接声明类
class	 INetMediator
{
public:
	INetMediator() {}
	virtual ~INetMediator() {}//使用时，父类指针指向子类，使用虚析构
	//初始化网络
	virtual bool OpenNet() = 0;
	//关闭网络
	virtual void CloseNet() = 0;
	//发送数据
	virtual bool SendData(long lSendIp, const char* buf, int nLen) = 0;
	//处理数据
	virtual void DealData(long lSendIp, const char* buf, int nLen) = 0;
protected:
	INet* m_pNet;//网络接口指针，用于调用网络函数
};	