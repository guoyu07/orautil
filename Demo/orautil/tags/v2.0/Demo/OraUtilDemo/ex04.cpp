// asppro.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include <orautil/orautil.hpp>
#include <iostream>
#include <assert.h>
#include <fstream>
#include <array>
#include <stdio.h>
#include <vector>
using namespace std;
using namespace asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// Execute()��ʹ��

static bool DropTable();
static bool CreateTable();
static bool InsertOneRow();
static int InsertRows_NoCommit();
static void PrintRowData(unsigned int printrowcout = 10);
// ɾ���ַ���β�����ܵķֺ�";"�����������
void TrimEndSemicolon(string& str);

// ����RAII�ַ�ʵ��"�Զ���ʼ��" ʾ��: 
class ASPAutoInit
{
public:
	ASPAutoInit() { ASPInitialize(); }
	~ASPAutoInit() { ASPCleanup(); }
};
static ASPAutoInit aspAutoInit;	

static ASPConnection aspCon;

int main()
{
	try
	{
		aspCon.Open("mybk106", "test123", "test123");
		cout << "�����ݿ�ɹ�\n";
	}
	catch (std::exception &ex)
	{
		cout << "�����ݿ�ʧ��:" << ex.what() << '\n';
		return 0;
	}

	DropTable();	
	CreateTable();
	InsertOneRow();
	int insertCnt = InsertRows_NoCommit();
	if (insertCnt > 0)
	{
		aspCon.Commit();
		cout << "������������ " << insertCnt << " ��\n";
		PrintRowData(insertCnt);
	}

	aspCon.Close();

	getchar();

	return 0;
}

bool DropTable()
{
	string sql = "drop table b_kkqp_jgcl";
	bool droped = false;
	try
	{
		aspCon.Execute(sql);
		droped = true;
		cout << "ɾ�����ݿ�ɹ�\n";
		aspCon.Commit();
	}
	catch (std::exception &ex)
	{
		cout << "ɾ����ʧ��:" << ex.what() << '\n';
	}

	return droped;
}

bool CreateTable()
{
	ifstream readFile("create_b_kkqp_jgcl.sql");
	if (!readFile.is_open())
	{
		cout << "�ļ� create_b_kkqp_jgcl ��ʧ��\n";
		return false;
	}

	string sql;
	while (!readFile.eof())
	{
		std::array<char , 256> line = {0};
		readFile.getline(line.data(), line.size());
		sql += line.data();
	}
	readFile.close();

	bool created = false;
	try
	{
		aspCon.Execute(sql);
		created = true;
		cout << "�������ݿ��ɹ�\n";
		aspCon.Commit();
	}
	catch (std::exception &ex)
	{
		cout << "�������ݿ��ʧ��:" << ex.what() << '\n'; 
	}

	return created;
}

bool InsertOneRow()
{
	string insert1 = "insert into b_kkqp_jgcl (JLBH, HPHM, HPZL, JGSJ, CLSD, ZGXS, ZDXS, "
		"CWKC, HPYS, CLLX, JLLX, HDSJ, BZWZDM, SBBH, CDBH, FXBH, TPSL, CLBJ, SHBJ, ZFBJ, "
		" SJLY, CCLX, BZ, TJBJ, CSYS, JLLB, XSFX, CSBJ, YZYS, TPLX, XRSJ, URL) values "
		" ('000000000000000000000000000', '��CT8283', 0, to_date('10-08-2016 00:03:53', "
		"'dd-mm-yyyy hh24:mi:ss'), 22, 80, 80, 3.00, '0', '2', '0', 610300, '610300001045',"
		"'610300001045', 2, 'EW', 1, null, '0', '0', '127.0.0.1', 1, null, null, 0, 0, 1, "
		"1, 1, 999, to_date('10-08-2016 00:44:40', 'dd-mm-yyyy hh24:mi:ss'), "
		"'/201605/25/08/12345678901-20160525083919-13-1.jpg;')";

	 bool inserted = false;
	 try
	 {
	 	aspCon.Execute(insert1);
		inserted = true;
		cout << "����һ�����ݳɹ�\n";
		aspCon.Commit();
	 }
	 catch (std::exception &ex)
	 {
		 cout << "����һ������ʧ��:" << ex.what() << '\n';
	 }

	 return inserted;
}

// ɾ���ַ���β�����ܵķֺ�";"�����������
void TrimEndSemicolon(string& str)
{
	string::size_type pos = str.find_last_of(";");
	if (pos != string::npos)
	{
		str = str.substr(0, pos);
	}
}

int InsertRows_NoCommit()
{
	ifstream readFile("insert_b_kkqp_jgcl.sql");
	if (!readFile.is_open())
	{
		cout << "�ļ� insert_b_kkqp_jgcl ��ʧ��\n";
		return -1;
	}

	string text;
	while (!readFile.eof())
	{
		std::array<char , 1024> line = {0};
		readFile.getline(line.data(), line.size());
		text += line.data();
	}

	vector<string> sqlList;
	string::size_type pos = string::npos;
	string::size_type pos2 = string::npos;

	do 
	{
		pos = text.find("insert into");
		if (pos == string::npos)
		{
			break;
		}

		pos2 = text.find("insert into", pos+1);
		if (pos2 != string::npos)
		{
			string tmp = text.substr(pos, pos2-pos);
			TrimEndSemicolon(tmp);
			sqlList.push_back(tmp);

			text = text.substr(pos2);
		}
		else
		{
			string tmp = text;
			TrimEndSemicolon(tmp);
			sqlList.push_back(tmp);

			break;
		}

	} while (true);

	readFile.close();

	const int count = sqlList.size();
	if (count == 0)
	{
		return -1;
	}

	try
	{
		aspCon.Execute(sqlList);
		return sqlList.size();
	}
	catch (std::exception&)
	{
		return -1;
	}
}

void PrintRowData(unsigned int printrowcout /* = 1 */)
{
	std::array<char, 256> szSql = {0};
	sprintf_s(szSql.data(), szSql.size(), 
		"select jlbh, hphm ,jgsj from b_kkqp_jgcl where rownum <= %d", printrowcout);

	ASPConnection con;
	ASPDataset dt;
	try
	{
		con.Open("mybk106", "test123", "test123");
		dt = con.Select(string(szSql.data()));
	}
	catch (std::exception &ex)
	{
		cout << "��ѯ���ݳ���:" << ex.what() << '\n';
		return;
	}

	cout << "���\t��¼���\t���ƺ���\t����ʱ��\n";

	int idx = 0;
	while (dt.Next())
	{
		try
		{
			string jlbl = dt.Get<string>(1);
			string hphm = dt.Get<string>(2);
			ASPDate jgsj = dt.Get<ASPDate>(3);

			cout << ++idx << ",\t" << jlbl << ",\t" << hphm << ",\t" 
				<< jgsj.ToString("yyyy-mm-dd hh24:mi:ss") << '\n';
		}
		catch (std::exception &ex)
		{
			cout << "��ȡ�ֶδ���:" << ex.what() << '\n';
			break;
		}
	}

	con.Close();
}





