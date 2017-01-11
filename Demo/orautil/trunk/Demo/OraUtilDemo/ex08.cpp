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

// ocilib::Date û���ṩ���"Сʱ", "����"��"��"����ļӼ�����, 
// �ṩ������������ AddHours(), AddMintues(), AddSeconds()

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

static ocilib::Pool aspPool;

int main()
{
	ocilib::Connection con;

	try
	{
		con.Open("mybk106", "test123", "test123");
		std::cout << "�����ݿ�ɹ�\n";
	}
	catch (std::exception &ex)
	{
		std::cout << "�����ݿ�ʧ��:" << ex.what() << '\n';
		return 0;
	}

	try
	{
		ocilib::Date dt = asp::SelectOne<ocilib::Date>(con, "select sysdate from dual");
		std::cout << dt.ToString("yyyy-mm-dd hh24:mi:ss") << '\n';

		asp::AddSeconds(dt, 20);
		std::cout << dt.ToString("yyyy-mm-dd hh24:mi:ss") << '\n';

		asp::AddSeconds(dt, -20);
		std::cout << dt.ToString("yyyy-mm-dd hh24:mi:ss") << '\n';

		asp::AddMintues(dt, 12);
		std::cout << dt.ToString("yyyy-mm-dd hh24:mi:ss") << '\n';

		asp::AddMintues(dt, -12);
		std::cout << dt.ToString("yyyy-mm-dd hh24:mi:ss") << '\n';

		asp::AddHours(dt, 1);
		std::cout << dt.ToString("yyyy-mm-dd hh24:mi:ss") << '\n';

		asp::AddHours(dt, -1);
		std::cout << dt.ToString("yyyy-mm-dd hh24:mi:ss") << '\n';

	}
	catch (std::exception &ex)
	{
		std::cout << ex.what() << '\n';
	}

	getchar();	

	return 0;
}





