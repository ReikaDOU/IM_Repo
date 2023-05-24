#include "Ckernel.h"
#include"TcpServerMediator.h"

CKernel* CKernel::p_kernel = NULL;
//设置协议映射关系
#define NetPackFunMap(a) m_netProtocalMap[a-DEF_PROTOCAL_BASE-10]

void CKernel::setProtocalMap()
{
	//初始化数组
	memset(m_netProtocalMap, 0, sizeof(m_netProtocalMap));

	NetPackFunMap(DEF_PACK_REGISTER_RQ) = &CKernel::DealRegisterRq;
	NetPackFunMap(DEF_PACK_LOGIN_RQ) = &CKernel::DealLoginRq;
	NetPackFunMap(DEF_PACK_CHAT_RQ) = &CKernel::dealChatRq;
}


CKernel::CKernel(void)
{
	CKernel::p_kernel = this;
	setProtocalMap();
}

CKernel::~CKernel(void)
{
	closeServer();
}

bool CKernel::startServer()
{
	//连接数据库
	m_pMySql = new CMySql;
	if (!m_pMySql->ConnectMySql("127.0.0.1", "root", "3032146556", "im"))
	{
		cout << "连接数据库失败" << endl;
		return false;
	}
	//打开网络
	m_pServer = new TcpServerMediator;
	if (!m_pServer->OpenNet())
	{
		cout << "打开网络失败" << endl;
	}
	return true;
}

void CKernel::closeServer()
{
	if (m_pMySql)
	{
		m_pMySql->DisConnect();
		delete m_pMySql;
		m_pMySql = NULL;
	}
	//关闭网络
	if (m_pServer)
	{
		m_pServer->CloseNet();
		delete m_pServer;
		m_pServer = NULL;
	}
}

void CKernel::DealData(long lSendIp, char* buf, int nLen)
{
	//获取协议头
	int type = *(int*)buf;
	//每一个协议头都会对应一个处理函数  映射->数组

	//判断协议在映射数组范围类
	if (type >= (DEF_PROTOCAL_BASE + 10) && 
		type < (DEF_PROTOCAL_BASE + 10 + DEF_PROTOCAL_COUNT))
	{
		//获取协议对应数组下标
		FUN pf = NetPackFunMap(type);
		//如果函数指针存在，调用对应的处理函数
		if (pf)
		{
			(this->*pf)(lSendIp, buf, nLen);
		}
	}
	//回收创建的空间
	delete[]buf;
}

void CKernel::DealLoginRq(long lSendIp, char* buf, int nLen)
{
	//1、拆包，获取tel，password
	STRU_LOGIN_RQ* rq = (STRU_LOGIN_RQ*)buf;
	//2、根据tel查询此人
	list<string> listRes;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select id,password from t_user where tel ='%s';", rq->tel);
	if (!m_pMySql->SelectMySql(sqlBuf, 2, listRes))
	{
		cout << "select error:" << sqlBuf << endl;
	}
	STRU_LOGIN_RS rs;
	int nId = 0;
	string strPass = "";
	if (listRes.size() == 0)
	{
		//3、无此人，直接返回用户不存在
		rs.result = user_not_exist;
		//发送登录结果
		m_pServer->SendData(lSendIp, (char*)&rs, sizeof(rs));
	}
	else
	{
		//4、有此人，获取用户ID和密码
		nId = atoi(listRes.front().c_str());
		listRes.pop_front();
		strPass = listRes.front().c_str();
		listRes.pop_front();
		//5、查看密码是否一致
		if (strcmp(strPass.c_str(), rq->password) != 0)
		{
			//6、不一致，返回密码错误
			rs.result = password_error;
			//发送登录结果
			m_pServer->SendData(lSendIp, (char*)&rs, sizeof(rs));
		}
		else
		{
			//7、密码一致，给客户端返回登录结果和用户id
			rs.result = login_success;
			rs.userid = nId;
			//发送登录结果
			m_pServer->SendData(lSendIp, (char*)&rs, sizeof(rs));

			//8、保存id 和 sock的映射关系、
			m_mapIdToSock[nId] = lSendIp;

			////9、获取好友列表
			getUserList(nId);
		}
	}
}

void CKernel::DealRegisterRq(long lSendIp, char* buf, int nLen)
{
	//1、拆包
	STRU_REGISTER_RQ* rq = (STRU_REGISTER_RQ*)buf;
	//2、拿到tel,以tel为条件查询数据库
	list<string> reList;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select tel from t_user where tel =%s", rq->tel);
	if (!m_pMySql->SelectMySql(sqlBuf, 1, reList))
	{
		cout << "查询数据库失败" << endl;
		return;
	}
	//3、判断是否已经有这人
	STRU_REGISTER_RS rs;
	if (reList.size() > 0)//有
	{
		//3·1、回复客户端该用户已存在
		rs.result = user_is_exist;
	}
	else//无
	{
		//4、添加到数据库
		rs.result = register_success;
		sprintf_s(sqlBuf, "insert into t_user(tel,password,name,icon,feeling)"
			"values('%s','%s','%s',1,'什么也没留下。。')", rq->tel, rq->password, rq->name);
		if (!m_pMySql->UpdateMySql(sqlBuf))
		{
			cout << "更新数据库失败" << endl;
			rs.result = register_fail;
		}
	}
	//5、给客户端发送注册结果
	m_pServer->SendData(lSendIp, (char*)&rs, sizeof(rs));
}

void CKernel::dealChatRq(long lSendIp, char* buf, int nLen)
{
	STRU_CHAT_RQ* rq = (STRU_CHAT_RQ*)buf;
	//查看对方是否在线
	if (m_mapIdToSock.find(rq->friendid) == m_mapIdToSock.end())
	{
		STRU_CHAT_RS rs;
		rs.userid = rq->userid;
		rs.friendid = rq->friendid;
		rs.result = user_offline;
		m_pServer->SendData(lSendIp, (char*)&rs, sizeof(rs));
	}
	else
	{
		SOCKET sockFriend = m_mapIdToSock[rq->friendid];
		m_pServer->SendData(sockFriend, buf, nLen);
	}
}

void CKernel::getUserList(int id)
{
	//查询自己信息
	STRU_FRIEND_INFO loginRq;
	getUserInfoFromSql(id, &loginRq);
	//取出自己sock
	if (m_mapIdToSock.find(id) == m_mapIdToSock.end())
	{
		return;
	}
	SOCKET sockLoginer = m_mapIdToSock[id];
	//发送自己 信息给客户端
	m_pServer->SendData(sockLoginer, (char*)&loginRq, sizeof(loginRq));
	//查询用户所有好友Id
	list<string>lstRes;
	char sqlbuf[1024] = "";
	sprintf_s(sqlbuf, "select idB from t_friend where idA = '%d';", id);
	if (!m_pMySql->SelectMySql(sqlbuf, 1, lstRes))
	{
		cout << "select error:" << sqlbuf << endl;
	}
	//遍历查询结果
	while (lstRes.size() > 0)
	{
		//根据id查询好友用户的好友信息
		int friendid = atoi(lstRes.front().c_str());
		lstRes.pop_front();
		STRU_FRIEND_INFO friendRq;
		getUserInfoFromSql(friendid, &friendRq);
		//给客户端发送该用户的好友信息
		m_pServer->SendData(sockLoginer, (char*)&friendRq, sizeof(friendRq));
		//如果好友在线，发送登录人的信息给好友
		if (m_mapIdToSock.find(friendid) == m_mapIdToSock.end())
		{
			continue;
		}
		//好友在线取出好友sock，通知好友登录人已上线
		SOCKET sockFriend = m_mapIdToSock[friendid];
		m_pServer->SendData(sockFriend, (char*)&loginRq, sizeof(loginRq));
	}

}
void CKernel::getUserInfoFromSql(int id, STRU_FRIEND_INFO* info)
{
	info->userid = id;
	//根据id查用户信息
	list<string> lstRes;
	char sqlbuf[1024] = "";
	sprintf_s(sqlbuf, "select name,icon,feeling from t_user where id = '%d';", id);
	if (!m_pMySql->SelectMySql(sqlbuf, 3, lstRes))
	{
		cout << "select error" << sqlbuf << endl;
	}
	if (lstRes.size() >0)
	{
		strcpy_s(info->name, lstRes.front().c_str());
		lstRes.pop_front();

		info->iconid = atoi(lstRes.front().c_str());
		lstRes.pop_front();

		strcpy_s(info->feeling, lstRes.front().c_str());
		lstRes.pop_front();
	}
	//查看用户是否在线
	if (m_mapIdToSock.find(id) != m_mapIdToSock.end())
	{
		info->state = 1;
	}
	else
	{
		info->state = 0;

	}
}