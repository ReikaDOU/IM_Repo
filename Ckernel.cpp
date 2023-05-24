#include "Ckernel.h"
#include"TcpServerMediator.h"

CKernel* CKernel::p_kernel = NULL;
//����Э��ӳ���ϵ
#define NetPackFunMap(a) m_netProtocalMap[a-DEF_PROTOCAL_BASE-10]

void CKernel::setProtocalMap()
{
	//��ʼ������
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
	//�������ݿ�
	m_pMySql = new CMySql;
	if (!m_pMySql->ConnectMySql("127.0.0.1", "root", "3032146556", "im"))
	{
		cout << "�������ݿ�ʧ��" << endl;
		return false;
	}
	//������
	m_pServer = new TcpServerMediator;
	if (!m_pServer->OpenNet())
	{
		cout << "������ʧ��" << endl;
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
	//�ر�����
	if (m_pServer)
	{
		m_pServer->CloseNet();
		delete m_pServer;
		m_pServer = NULL;
	}
}

void CKernel::DealData(long lSendIp, char* buf, int nLen)
{
	//��ȡЭ��ͷ
	int type = *(int*)buf;
	//ÿһ��Э��ͷ�����Ӧһ��������  ӳ��->����

	//�ж�Э����ӳ�����鷶Χ��
	if (type >= (DEF_PROTOCAL_BASE + 10) && 
		type < (DEF_PROTOCAL_BASE + 10 + DEF_PROTOCAL_COUNT))
	{
		//��ȡЭ���Ӧ�����±�
		FUN pf = NetPackFunMap(type);
		//�������ָ����ڣ����ö�Ӧ�Ĵ�����
		if (pf)
		{
			(this->*pf)(lSendIp, buf, nLen);
		}
	}
	//���մ����Ŀռ�
	delete[]buf;
}

void CKernel::DealLoginRq(long lSendIp, char* buf, int nLen)
{
	//1���������ȡtel��password
	STRU_LOGIN_RQ* rq = (STRU_LOGIN_RQ*)buf;
	//2������tel��ѯ����
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
		//3���޴��ˣ�ֱ�ӷ����û�������
		rs.result = user_not_exist;
		//���͵�¼���
		m_pServer->SendData(lSendIp, (char*)&rs, sizeof(rs));
	}
	else
	{
		//4���д��ˣ���ȡ�û�ID������
		nId = atoi(listRes.front().c_str());
		listRes.pop_front();
		strPass = listRes.front().c_str();
		listRes.pop_front();
		//5���鿴�����Ƿ�һ��
		if (strcmp(strPass.c_str(), rq->password) != 0)
		{
			//6����һ�£������������
			rs.result = password_error;
			//���͵�¼���
			m_pServer->SendData(lSendIp, (char*)&rs, sizeof(rs));
		}
		else
		{
			//7������һ�£����ͻ��˷��ص�¼������û�id
			rs.result = login_success;
			rs.userid = nId;
			//���͵�¼���
			m_pServer->SendData(lSendIp, (char*)&rs, sizeof(rs));

			//8������id �� sock��ӳ���ϵ��
			m_mapIdToSock[nId] = lSendIp;

			////9����ȡ�����б�
			getUserList(nId);
		}
	}
}

void CKernel::DealRegisterRq(long lSendIp, char* buf, int nLen)
{
	//1�����
	STRU_REGISTER_RQ* rq = (STRU_REGISTER_RQ*)buf;
	//2���õ�tel,��telΪ������ѯ���ݿ�
	list<string> reList;
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select tel from t_user where tel =%s", rq->tel);
	if (!m_pMySql->SelectMySql(sqlBuf, 1, reList))
	{
		cout << "��ѯ���ݿ�ʧ��" << endl;
		return;
	}
	//3���ж��Ƿ��Ѿ�������
	STRU_REGISTER_RS rs;
	if (reList.size() > 0)//��
	{
		//3��1���ظ��ͻ��˸��û��Ѵ���
		rs.result = user_is_exist;
	}
	else//��
	{
		//4����ӵ����ݿ�
		rs.result = register_success;
		sprintf_s(sqlBuf, "insert into t_user(tel,password,name,icon,feeling)"
			"values('%s','%s','%s',1,'ʲôҲû���¡���')", rq->tel, rq->password, rq->name);
		if (!m_pMySql->UpdateMySql(sqlBuf))
		{
			cout << "�������ݿ�ʧ��" << endl;
			rs.result = register_fail;
		}
	}
	//5�����ͻ��˷���ע����
	m_pServer->SendData(lSendIp, (char*)&rs, sizeof(rs));
}

void CKernel::dealChatRq(long lSendIp, char* buf, int nLen)
{
	STRU_CHAT_RQ* rq = (STRU_CHAT_RQ*)buf;
	//�鿴�Է��Ƿ�����
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
	//��ѯ�Լ���Ϣ
	STRU_FRIEND_INFO loginRq;
	getUserInfoFromSql(id, &loginRq);
	//ȡ���Լ�sock
	if (m_mapIdToSock.find(id) == m_mapIdToSock.end())
	{
		return;
	}
	SOCKET sockLoginer = m_mapIdToSock[id];
	//�����Լ� ��Ϣ���ͻ���
	m_pServer->SendData(sockLoginer, (char*)&loginRq, sizeof(loginRq));
	//��ѯ�û����к���Id
	list<string>lstRes;
	char sqlbuf[1024] = "";
	sprintf_s(sqlbuf, "select idB from t_friend where idA = '%d';", id);
	if (!m_pMySql->SelectMySql(sqlbuf, 1, lstRes))
	{
		cout << "select error:" << sqlbuf << endl;
	}
	//������ѯ���
	while (lstRes.size() > 0)
	{
		//����id��ѯ�����û��ĺ�����Ϣ
		int friendid = atoi(lstRes.front().c_str());
		lstRes.pop_front();
		STRU_FRIEND_INFO friendRq;
		getUserInfoFromSql(friendid, &friendRq);
		//���ͻ��˷��͸��û��ĺ�����Ϣ
		m_pServer->SendData(sockLoginer, (char*)&friendRq, sizeof(friendRq));
		//����������ߣ����͵�¼�˵���Ϣ������
		if (m_mapIdToSock.find(friendid) == m_mapIdToSock.end())
		{
			continue;
		}
		//��������ȡ������sock��֪ͨ���ѵ�¼��������
		SOCKET sockFriend = m_mapIdToSock[friendid];
		m_pServer->SendData(sockFriend, (char*)&loginRq, sizeof(loginRq));
	}

}
void CKernel::getUserInfoFromSql(int id, STRU_FRIEND_INFO* info)
{
	info->userid = id;
	//����id���û���Ϣ
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
	//�鿴�û��Ƿ�����
	if (m_mapIdToSock.find(id) != m_mapIdToSock.end())
	{
		info->state = 1;
	}
	else
	{
		info->state = 0;

	}
}