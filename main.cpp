#include"TcpClientMediator.h"
#include"TcpServerMediator.h"
#include<Windows.h>
#include<iostream>
#include"CMySql.h"
using namespace std;

int main()
{
	INetMediator* m_Server = new TcpServerMediator;
	if (!m_Server->OpenNet())
	{
		cout << "服务器开启失败" << endl;
	}

	INetMediator* m_Client = new TcpClientMediator;
	if (!m_Client->OpenNet())
	{
		cout << "服务器开启失败" << endl;
	}
	char a[100] = "hello world";
	m_Client->SendData(0, a, strlen("hello world") + 1);

	while (1)
	{
		Sleep(10);
		

	}

	system("pause");
	return 0;
}