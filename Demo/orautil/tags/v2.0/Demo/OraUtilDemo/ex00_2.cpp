// asppro::oracle.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include <orautil/orautil.hpp>
#include <iostream>
using namespace std;
using namespace asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// ���ݿ⻷����ʼ����ע��

// ����RAII�ַ�ʵ��"�Զ���ʼ����ע��" ʾ��: 
class ASPAutoInit
{
public:
	ASPAutoInit() { ASPInitialize(); }
	~ASPAutoInit() { ASPCleanup(); }
};
static ASPAutoInit aspAutoInit;	

int main()
{
	// ���ݿ���ز���
	// ...
	// ...

	return 0;
}

