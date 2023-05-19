#include<iostream>
#include"CMySql.h"
using namespace std;

struct book
{
	string num;
	string name;
	string author;
	string price;
	string note;
};

int main()
{
	CMySql sql;
	if(!sql.ConnectMySql("127.0.0.1","root","3032146556","lianxi"))
	{
		cout << "database open failed" << endl;
		system("pause");
		return 0;
	}

	//------------------
	list<string>lstRes;
	char sqlbuf[1024] = "";
	sprintf_s(sqlbuf, "select * from t_book;");
	if (!sql.SelectMySql(sqlbuf, 5, lstRes))
	{
		cout << "database select failed" << endl;
	}

	struct book bk = {};
	while (lstRes.size() > 0)
	{
		bk.num = lstRes.front();
		lstRes.pop_front();
		bk.name = lstRes.front();
		lstRes.pop_front();
		bk.author = lstRes.front();
		lstRes.pop_front();
		bk.price = lstRes.front();
		lstRes.pop_front();
		bk.note = lstRes.front();
		lstRes.pop_front();
		cout << "num:" << bk.num.c_str()
			<< ",name:" << bk.name.c_str()
			<< ",author:" << bk.author.c_str()
			<< ",price:" << bk.price.c_str()
			<< ",note:" << bk.note.c_str() << endl;
	}

	//------------------

	sprintf_s(sqlbuf, "update t_book set b_name = '%s' where b_num = '%s';", "Êý¾Ý¿â»ù´¡", "2");
	if (!sql.UpdateMySql(sqlbuf))
	{
		cout << "database update failed: " << sqlbuf << endl;
	}
	else
	{
		cout << "database update successed" << endl;
	}

	//------------------

	sprintf_s(sqlbuf, "delete from t_book where b_num = '%s';", "8");
	if (!sql.UpdateMySql(sqlbuf))
	{
		cout << "database delete failed: " << sqlbuf << endl;
	}
	else
	{
		cout << "database delete successed" << endl;
	}





	system("pause");
	sql.DisConnect();
	return 0;
}