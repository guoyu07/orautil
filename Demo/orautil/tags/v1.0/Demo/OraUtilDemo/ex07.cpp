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

// ���̲߳�������

#define local static

local CRITICAL_SECTION cs; 

local void MutilThreadInset(void* pThreadIdx);

int main()
{
	try
	{
		//OracleUtil::OpenDB("���ݿ������", "�û���", "����");
		OracleUtil::OpenDB("mybk106", "test123", "test123", 1, 10);
		cout << "�����ݿ�ɹ�" << endl;
	}
	catch (std::exception &ex)
	{
		cout << "�����ݿ�ʧ��:" << ex.what() << endl;
		return 0;
	}

	OracleUtil ora;

	try
	{
		ora.Execute("drop table b_mutltest");
		cout << "ɾ����ɹ�" << endl;
	}
	catch (std::exception) 
	{ }

	try
	{
		ora.Execute("create table b_mutltest (jlbh varchar2(32) not null, xrsj date default sysdate, threadidx number(6))");
		cout << "������ɹ�" << endl;
	}
	catch (std::exception &ex)
	{
		cout << "������ʧ��:" << ex.what() << endl;
		return 0;
	}

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

	std::unique_ptr<Dataset> dt = ora.Select2("SELECT count(*), MAX(xrsj), MIN(xrsj), (MAX(xrsj) - MIN(xrsj)) * 24 * 3600 FROM b_mutltest");

	ora.Select2("SELECT count(*), MAX(xrsj), MIN(xrsj), (MAX(xrsj) - MIN(xrsj)) * 24 * 3600 FROM b_mutltest");

	dt->Next();
	int count = dt->Get<DTInt>(1);
	DTDate maxXRSJ = dt->Get<DTDate>(2);
	DTDate minXRSJ = dt->Get<DTDate>(3);
	int secodes = dt->Get<DTInt>(4);
	cout << "��������\t" << "���д��ʱ��\t" << "��Сд��ʱ��\t" << "д���ʱ" << endl;
	cout << count << ",\t" << maxXRSJ.ToString("yyyy-mm-dd hh24:mi:ss")
		<< ",\t" << minXRSJ.ToString("yyyy-mm-dd hh24:mi:ss")
		<< ",\t" << secodes << endl;

	getchar();	

	return 0;
}

void MutilThreadInset(void* pThradIdx)
{
	int id = * (static_cast<int*>(pThradIdx));

	OracleUtil ora;
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
			ora.Execute(string(szSql.data()));	
		}
		catch (std::exception &ex)
		{
			cout << "�������ݴ���, �߳�idx:" << id << ", ����:" << ex.what() << endl;
			break;
		}

		if (counter == 10000)
		{
			cout << "�߳�:" << id << " ����ִ�����" << endl;
			break;
		}
	}
}



