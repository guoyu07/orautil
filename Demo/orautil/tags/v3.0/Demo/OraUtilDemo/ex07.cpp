// asppro.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include <orautil/orautil.hpp>
#include <iostream>
#include <assert.h>
#include <array>
#include <stdio.h>
#include <windows.h>
#include <process.h>
namespace asp = asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// Bind �󶨲�����ʾ

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
static asp::OciEnvInit ociEnvInit;

static asp::AutoPool aspPool(new ocilib::Pool(), asp::PoolDeleter);

int main()
{
	asp::AutoConnection con(new ocilib::Connection(), asp::ConnectionDeleter);

	try
	{
		con->Open("mybk106", "test123", "test123");
		std::cout << "�����ݿ�ɹ�\n";
	}
	catch (std::exception &ex)
	{
		std::cout << "�����ݿ�ʧ��:" << ex.what() << '\n';
		return 0;
	}

	ocilib::Statement st(*con.get());

	try
	{
		st.Execute("drop table b_mutltest");
		con->Commit();
		std::cout << "ɾ����ɹ�\n";
	}
	catch (std::exception) 
	{ }

	try
	{
		st.Execute(
			"create table b_mutltest "
			"("
			"jlbh varchar2(32) not null,"
			"xrsj date default sysdate,"
			"threadidx number(6)"
			")");
		con->Commit();
		std::cout << "������ɹ�\n";
	}
	catch (std::exception &ex)
	{
		std::cout << "������ʧ��:" << ex.what() << '\n';
		return 0;
	}

	// Ϊ���̲߳������ݿ��������"���ӳ�"
	try
	{
		aspPool->Open("mybk106", "test123", "test123", ocilib::Pool::SessionPool, 1, 5);
		std::cout << "���ӳش����ɹ�\n";
	}
	catch (std::exception &ex)
	{
		std::cout << "���ӳش���ʧ��:" << ex.what() << '\n';
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
			std::cout << "�����¼���ʧ��\n";
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
	std::cout << "�����̳߳سɹ�\n";	

	// MAXIMUM_WAIT_OBJECTS = 64
	static_assert(TotalNum <= MAXIMUM_WAIT_OBJECTS, 
		"The max-number of WaitForMultipleObjects() to wait for "
		"event-object is only MAXIMUM_WAIT_OBJECTS");
	int idx = 0;
	while (WaitForMultipleObjects(TotalNum, events, TRUE, 1000) == WAIT_TIMEOUT)
	{
		std::cout << ++idx << " >>>>>>>>>>>>>>>>>>>>>>>>�ȴ��������...\n";
	}
	DWORD lasterr = GetLastError();
	std::cout << "�����������, LastError:" << lasterr << '\n';

	DeleteCriticalSection(&cs);

	for (int i = 0; i < TotalNum; ++i)
	{
		CloseHandle(events[i]);
	}

	std::unique_ptr<ocilib::Resultset> rs = NULL;
	try
	{
		st.Execute(
			"SELECT count(*), MAX(xrsj), MIN(xrsj), (MAX(xrsj) - MIN(xrsj)) * 24 * 3600 "
			"FROM b_mutltest");
		rs = std::unique_ptr<ocilib::Resultset>(new ocilib::Resultset(st.GetResultset()));

		rs->Next();
		int count = rs->Get<int>(1);
		ocilib::Date maxXRSJ = rs->Get<ocilib::Date>(2);
		ocilib::Date minXRSJ = rs->Get<ocilib::Date>(3);
		int secodes = rs->Get<int>(4);
		std::cout << "��������\t" << "���д��ʱ��\t" << "��Сд��ʱ��\t" << "д���ʱ\n";
		std::cout << count << ",\t" << maxXRSJ.ToString("yyyy-mm-dd hh24:mi:ss")
			<< ",\t" << minXRSJ.ToString("yyyy-mm-dd hh24:mi:ss")
			<< ",\t" << secodes << '\n';
	}
	catch (std::exception &ex)
	{
		std::cout << "��ѯ��ȡ���ݳ���:" << ex.what() << '\n';
	}

	getchar();	

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
		std::string jlbh(szJLBH.data());
		ocilib::Date dt = asp::Time2Date(time(NULL));

		try
		{
			ocilib::Connection con = aspPool->GetConnection();
			ocilib::Statement st(con);
			st.Prepare("insert into b_mutltest(jlbh, xrsj, threadidx) values (:jlbh, :xrsj, :threadidx)");
			st.Bind(":jlbh", jlbh, jlbh.size(), ocilib::BindInfo::In);
			st.Bind(":xrsj", dt, ocilib::BindInfo::In);
			st.Bind(":threadidx", id, ocilib::BindInfo::In);
			st.ExecutePrepared();
			con.Commit();
			con.Close();
		}
		catch (std::exception &ex)
		{
			std::cout << "�������ݴ���, �߳�idx:" << id << ", ����:" << ex.what() << '\n';
			break;
		}

		if (counter == 1000)
		{
			EnterCriticalSection(&cs);
			std::cout << "�߳�:" << id << " ����ִ�����\n";
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



