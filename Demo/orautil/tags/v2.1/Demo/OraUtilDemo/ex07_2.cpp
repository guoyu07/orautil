// asppro.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include <orautil/orautil.hpp>
#include <iostream>
#include <assert.h>
#include <array>
#include <stdio.h>
#include <windows.h>
#include <process.h>
using namespace std;
using namespace asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// ocilib ���̲߳�������, �Ա���orautil��װ�����Ƚ�

static CRITICAL_SECTION cs; 
static void MutilThreadInset(void* pThreadIdx);
static std::string Time2Str(const time_t& time, 
	const std::string& format = "yyyy-mm-dd hh24:mi:ss");

// ����RAII�ַ�ʵ��"�Զ���ʼ��" ʾ��: 
class AutoInit
{
public:
	AutoInit()
	{
		ocilib::Environment::Initialize(ocilib::Environment::Default | ocilib::Environment::Threaded);
		ocilib::Environment::EnableWarnings(true);
	}

	~AutoInit()
	{
		ocilib::Environment::Cleanup(); 
	}
};
static AutoInit autoInit;

static ocilib::Pool pool;

int main()
{
	try
	{
		pool.Open("mybk106", "test123", "test123", ocilib::Pool::SessionPool, 1, 10);

		ocilib::Connection con = pool.GetConnection();
		ocilib::Statement st(con);
		st.Execute("drop table b_mutltest");
		st.Execute("create table b_mutltest (jlbh varchar2(32) not null, xrsj date default sysdate, threadidx number(6))");

		InitializeCriticalSection(&cs);

		// ����100���߳�
		const int threadNum = 100;
		int threadIdx[threadNum] = {0};
		for (int i  = 0; i < threadNum; ++i)
		{
			threadIdx[i] = i + 1;
			_beginthread(MutilThreadInset, 0, &(threadIdx[i]));
		}
		cout << "�����̳߳سɹ�" << endl;	

		getchar();		// �ȴ��������

		DeleteCriticalSection(&cs);

		st.Execute("SELECT count(*), MAX(xrsj), MIN(xrsj), (MAX(xrsj) - MIN(xrsj)) * 24 * 3600 FROM b_mutltest");
		ocilib::Resultset set = st.GetResultset();
		set.Next();
		int count = set.Get<int>(1);
		ocilib::Date maxXRSJ = set.Get<ocilib::Date>(2);
		ocilib::Date minXRSJ = set.Get<ocilib::Date>(3);
		int secodes = set.Get<int>(4);
		cout << "��������\t" << "���д��ʱ��\t" << "��Сд��ʱ��\t" << "д���ʱ" << endl;
		cout << count << ",\t" << maxXRSJ.ToString("yyyy-mm-dd hh24:mi:ss")
			<< ",\t" << minXRSJ.ToString("yyyy-mm-dd hh24:mi:ss")
			<< ",\t" << secodes << endl;
	}
	catch (std::exception &ex)
	{
		cout << "�����쳣����:" << ex.what() << endl;
	}

	getchar();	

	return 0;
}

std::string Time2Str(const time_t& time, const std::string& format /*= "yyyy-mm-dd hh24:mi:ss" */)
{
	otext chTmp[100] = {0};
	struct tm mytm;
	localtime_s(&mytm, &time);
	mytm.tm_year = mytm.tm_year + 1900;
	mytm.tm_mon = mytm.tm_mon + 1;

	_snprintf_s(chTmp, _countof(chTmp), "%04d-%02d-%02d %02d:%02d:%02d",
		mytm.tm_year, mytm.tm_mon, mytm.tm_mday, mytm.tm_hour, mytm.tm_min, mytm.tm_sec);

	ocilib::Date date(std::string(chTmp), "yyyy-mm-dd hh24:mi:ss");
	return date.ToString(format);
}

void MutilThreadInset(void* pThradIdx)
{
	int id = * (static_cast<int*>(pThradIdx));

	static int counter = 0;
	std::array<char, 50> szJLBH = {0};
	std::array<char, 256> szSql = {0};

	while (true)
	{
		EnterCriticalSection(&cs);
		counter += 1;
		LeaveCriticalSection(&cs);		

		sprintf_s(szJLBH.data(), szJLBH.size(), "%04d%08d", id, counter);

		time_t tt = time(NULL);

		sprintf_s(szSql.data(), szSql.size(), 
			"insert into b_mutltest(jlbh, xrsj, threadidx) values (%s, to_date('%s', 'yyyy-mm-dd hh24:mi:ss'), %d)",
			szJLBH.data(), Time2Str(tt, "yyyy-mm-dd hh24:mi:ss").c_str(), id);

		try
		{
			ocilib::Connection con = pool.GetConnection();
			ocilib::Statement st(con);
			st.Execute(string(szSql.data()));	
			con.Commit();
			con.Close();
		}
		catch (std::exception &ex)
		{
			cout << "�������ݴ���, �߳�idx:" << id << ", ����:" << ex.what() << endl;
			break;
		}

		if (counter == 1000)
		{
			cout << "�߳�:" << id << " ����ִ�����" << endl;
			break;
		}
	}
}



