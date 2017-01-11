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

// ���̲߳�������, WaitForMultipleObjects()�ȴ��߳����

#define USE_CSTYLE_THREAD	1

static CRITICAL_SECTION cs; 
static const int TotalNum = 60;
static HANDLE events[TotalNum] = {NULL};
static DWORD threadsID[TotalNum] = {0};

#if USE_CSTYLE_THREAD
static void MutilThreadInset(void* pThreadNum);
#else
static DWORD WINAPI MutilThreadInset(void* pThreadNum);
#endif

// ����RAII�ַ�ʵ��"�Զ���ʼ��" ʾ��: 
class ASPAutoInit
{
public:
	ASPAutoInit() { ASPInitialize(); }
	~ASPAutoInit() { ASPCleanup(); }
};
static ASPAutoInit aspAutoInit;	

static ASPPool aspPool;

int main()
{
	ASPConnection con;
	try
	{
		con.Open("mybk106", "test123", "test123");
		cout << "�����ݿ�ɹ�\n";
	}
	catch (std::exception &ex)
	{
		cout << "�����ݿ�ʧ��:" << ex.what() << endl;
		return 0;
	}

	try
	{
		con.Execute("drop table b_mutltest");
		con.Commit();
		cout << "ɾ����ɹ�\n";
	}
	catch (std::exception) 
	{ }

	try
	{
		con.Execute(
			"create table b_mutltest "
			"("
				"jlbh varchar2(32) not null,"
				"xrsj date default sysdate,"
				"threadidx number(6)"
			")");
		con.Commit();
		cout << "������ɹ�\n";
	}
	catch (std::exception &ex)
	{
		cout << "������ʧ��:" << ex.what() << endl;
		return 0;
	}

	// Ϊ���̲߳������ݿ��������"���ӳ�"
	try
	{
		aspPool.Open("mybk106", "test123", "test123", 1, 5);
		cout << "���ӳش����ɹ�\n";
	}
	catch (std::exception &ex)
	{
		cout << "���ӳش���ʧ��:" << ex.what() << '\n';
		return 0;
	}

	InitializeCriticalSection(&cs);

	bool createEventGroupSucc = true;
	for (int i = 0; i < TotalNum; ++i)
	{
		events[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (events[i] == NULL)
		{
			createEventGroupSucc = false;
			cout << "�����¼���ʧ��\n";
			break;
		}
	}

	if (!createEventGroupSucc)
	{
		for (int i = 0; i < TotalNum; ++i)
		{
			if (events[i] != NULL)
			{
				CloseHandle(events);
			}
		}

		return 0;
	}

	// ����100���߳�	
	int threadNum[TotalNum] = {0};
	for (int i  = 0; i < TotalNum; ++i)
	{
		threadNum[i] = i;
#if USE_CSTYLE_THREAD
		_beginthread(MutilThreadInset, 0, &(threadNum[i]));
#else
		CreateThread(NULL, 0, MutilThreadInset, &(threadNum[i]), 0, &(threadsID[i]));
#endif
	}
	cout << "�����̳߳سɹ�\n";	

	// MAXIMUM_WAIT_OBJECTS = 64
	static_assert(TotalNum <= MAXIMUM_WAIT_OBJECTS, 
		"The max-number of WaitForMultipleObjects() to wait for "
		"event-object is only MAXIMUM_WAIT_OBJECTS");
	int idx = 0;
	while (WaitForMultipleObjects(TotalNum, events, TRUE, 1000) == WAIT_TIMEOUT)
	{
		cout << ++idx << " >>>>>>>>>>>>>>>>>>>>>>>>�ȴ��������...\n";
	}
	DWORD lasterr = GetLastError();
	cout << "�����������, LastError:" << lasterr << endl;

	DeleteCriticalSection(&cs);

	for (int i = 0; i < TotalNum; ++i)
	{
		CloseHandle(events[i]);
	}

	try
	{
		ASPDataset dt = con.Select(
			"SELECT count(*), MAX(xrsj), MIN(xrsj), (MAX(xrsj) - MIN(xrsj)) * 24 * 3600 "
			"FROM b_mutltest");

		dt.Next();
		int count = dt.Get<int>(1);
		ASPDate maxXRSJ = dt.Get<ASPDate>(2);
		ASPDate minXRSJ = dt.Get<ASPDate>(3);
		int secodes = dt.Get<int>(4);
		cout << "��������\t" << "���д��ʱ��\t" << "��Сд��ʱ��\t" << "д���ʱ\n";
		cout << count << ",\t" << maxXRSJ.ToString("yyyy-mm-dd hh24:mi:ss")
			<< ",\t" << minXRSJ.ToString("yyyy-mm-dd hh24:mi:ss")
			<< ",\t" << secodes << endl;
	}
	catch (std::exception &ex)
	{
		cout << "��ѯ��ȡ���ݳ���:" << ex.what() << '\n';
	}

	con.Close();

	getchar();	

	aspPool.Close();		// ���Բ�����ʽ����, ASPPool�����������Զ�����

	return 0;
}

#if USE_CSTYLE_THREAD
void MutilThreadInset(void* pThreadNum)
#else
DWORD WINAPI MutilThreadInset(void* pThreadNum)
#endif
{
	int id = * (static_cast<int*>(pThreadNum));

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
			szJLBH.data(), ASPDate::Time2Str(tt, "yyyy-mm-dd hh24:mi:ss").c_str(), id);

		try
		{
			ASPConnection con = aspPool.GetConnection();
			con.Execute(string(szSql.data()));
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
			EnterCriticalSection(&cs);
			cout << "�߳�:" << id << " ����ִ�����\n";
			LeaveCriticalSection(&cs);	
			break;
		}
	}

	EnterCriticalSection(&cs);
	BOOL ret = SetEvent(events[id]);
	assert(ret);
	LeaveCriticalSection(&cs);	

#if !USE_CSTYLE_THREAD
	return 0;
#endif
}



