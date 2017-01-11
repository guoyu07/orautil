// asppro.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include <orautil/orautil.hpp>
#include <iostream>
#include <assert.h>
#include <fstream>
#include <array>
#include <stdio.h>
#include <vector>
namespace asp = asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// Execute, Select��ʹ��

static bool DropTable();
static bool CreateTable();
static bool InsertOneRow();
static int InsertRows_NoCommit();
static void PrintRowData(unsigned int printRows = 10);

// ɾ���ַ���β�����ܵķֺ�";"�����������
void TrimEndSemicolon(std::string& str);

// ����RAII�ַ�ʵ��"�Զ���ʼ��" ʾ��: 
static asp::OciEnvInit ociEnvInit;

static asp::AutoConnection aspCon(new ocilib::Connection(), asp::ConnectionDeleter);

int main()
{
	try
	{
		aspCon->Open("mybk106", "test123", "test123");
		std::cout << "�����ݿ�ɹ�\n";
	}
	catch (std::exception &ex)
	{
		std::cout << "�����ݿ�ʧ��:" << ex.what() << '\n';
		return 0;
	}

	DropTable();	
	CreateTable();
	InsertOneRow();
	int insertCnt = InsertRows_NoCommit();
	if (insertCnt > 0)
	{
		aspCon->Commit();
		std::cout << "������������ " << insertCnt << " ��\n";
		PrintRowData(insertCnt);
	}

	aspCon->Close();

	getchar();

	return 0;
}

bool DropTable()
{
	std::string sql = "drop table b_kkqp_jgcl";
	bool droped = false;
	try
	{
		ocilib::Statement st(*aspCon.get());
		st.Execute(sql);
		droped = true;
		std::cout << "ɾ�����ݿ�ɹ�\n";
		aspCon->Commit();
	}
	catch (std::exception &ex)
	{
		std::cout << "ɾ����ʧ��:" << ex.what() << '\n';
	}

	return droped;
}

bool CreateTable()
{
	std::ifstream readFile("create_b_kkqp_jgcl.sql");
	if (!readFile.is_open())
	{
		std::cout << "�ļ� create_b_kkqp_jgcl ��ʧ��\n";
		return false;
	}

	std::string sql;
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
		ocilib::Statement st(*aspCon.get());
		st.Execute(sql);
		created = true;
		std::cout << "�������ݿ��ɹ�\n";
		aspCon->Commit();
	}
	catch (std::exception &ex)
	{
		std::cout << "�������ݿ��ʧ��:" << ex.what() << '\n'; 
	}

	return created;
}

bool InsertOneRow()
{
	std::string sql = "insert into b_kkqp_jgcl (JLBH, HPHM, HPZL, JGSJ, CLSD, ZGXS, ZDXS, "
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
	 	ocilib::Statement st(*aspCon.get());
		st.Execute(sql);
		inserted = true;
		std::cout << "����һ�����ݳɹ�\n";
		aspCon->Commit();
	 }
	 catch (std::exception &ex)
	 {
		 std::cout << "����һ������ʧ��:" << ex.what() << '\n';
	 }

	 return inserted;
}

// ɾ���ַ���β�����ܵķֺ�";"�����������
void TrimEndSemicolon(std::string& str)
{
	std::string::size_type pos = str.find_last_of(";");
	if (pos != std::string::npos)
	{
		str = str.substr(0, pos);
	}
}

int InsertRows_NoCommit()
{
	std::ifstream readFile("insert_b_kkqp_jgcl.sql");
	if (!readFile.is_open())
	{
		std::cout << "�ļ� insert_b_kkqp_jgcl ��ʧ��\n";
		return -1;
	}

	std::string text;
	while (!readFile.eof())
	{
		std::array<char , 1024> line = {0};
		readFile.getline(line.data(), line.size());
		text += line.data();
	}

	std::vector<std::string> sqlList;
	std::string::size_type pos = std::string::npos;
	std::string::size_type pos2 = std::string::npos;

	do 
	{
		pos = text.find("insert into");
		if (pos == std::string::npos)
		{
			break;
		}

		pos2 = text.find("insert into", pos+1);
		if (pos2 != std::string::npos)
		{
			std::string tmp = text.substr(pos, pos2-pos);
			TrimEndSemicolon(tmp);
			sqlList.push_back(tmp);

			text = text.substr(pos2);
		}
		else
		{
			std::string tmp = text;
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

	ocilib::Statement st(*aspCon.get());

	try
	{
		for (std::vector<std::string>::iterator iter = sqlList.begin(); 
			iter != sqlList.end(); ++iter)
		{
			st.Execute(*iter);
		}

		return sqlList.size();
	}
	catch (std::exception&)
	{
		return -1;
	}
}

void PrintRowData(unsigned int printRows /* = 1 */)
{
	std::array<char, 256> szSql = {0};
	sprintf_s(szSql.data(), szSql.size(), 
		"select jlbh, hphm ,jgsj from b_kkqp_jgcl where rownum <= %d", printRows);

	asp::AutoConnection con(new ocilib::Connection(), asp::ConnectionDeleter);
	ocilib::Statement st(*con.get());
	std::unique_ptr<ocilib::Resultset> rs = NULL;

	try
	{
		con->Open("mybk106", "test123", "test123");
		st.Execute(szSql.data());
		rs = std::unique_ptr<ocilib::Resultset>(new ocilib::Resultset(st.GetResultset()));
	}
	catch (std::exception &ex)
	{
		std::cout << "��ѯ���ݳ���:" << ex.what() << '\n';
		return;
	}

	std::cout << "���\t��¼���\t���ƺ���\t����ʱ��\n";

	int idx = 0;
	while (rs->Next())
	{
		try
		{
			std::string jlbl = rs->Get<std::string>(1);
			std::string hphm = rs->Get<std::string>(2);
			ocilib::Date jgsj = rs->Get<ocilib::Date>(3);

			std::cout << ++idx << ",\t" << jlbl << ",\t" << hphm << ",\t" 
				<< jgsj.ToString("yyyy-mm-dd hh24:mi:ss") << '\n';
		}
		catch (std::exception &ex)
		{
			std::cout << "��ȡ�ֶδ���:" << ex.what() << '\n';
			break;
		}
	}
}





