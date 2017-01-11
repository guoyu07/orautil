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

#define local static

local CRITICAL_SECTION cs; 

local void MutilThreadInset(void* pThreadIdx);

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

local ocilib::Pool pool;

int main()
{
	try
	{
		AutoInit autoInit;

		pool.Open("mybk106", "test123", "test123", ocilib::Pool::SessionPool, 10, 10+1);

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
		int count = set.Get<DTInt>(1);
		DTDate maxXRSJ = set.Get<DTDate>(2);
		DTDate minXRSJ = set.Get<DTDate>(3);
		int secodes = set.Get<DTInt>(4);
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



