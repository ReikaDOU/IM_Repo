#pragma once
class INet;//������ͷ�ļ���ֱ��������
class	 INetMediator
{
public:
	INetMediator() {}
	virtual ~INetMediator() {}//ʹ��ʱ������ָ��ָ�����࣬ʹ��������
	//��ʼ������
	virtual bool OpenNet() = 0;
	//�ر�����
	virtual void CloseNet() = 0;
	//��������
	virtual bool SendData(long lSendIp, const char* buf, int nLen) = 0;
	//��������
	virtual void DealData(long lSendIp, const char* buf, int nLen) = 0;
protected:
	INet* m_pNet;//����ӿ�ָ�룬���ڵ������纯��
};	