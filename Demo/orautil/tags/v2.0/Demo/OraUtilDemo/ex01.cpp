// asppro::oracle.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include <orautil/orautil.hpp>
#include <iostream>
using namespace std;
using namespace asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// ��, �ر����ݿ� ��ʾ

// ����RAII�ַ�ʵ��"�Զ���ʼ��" ʾ��: 
class ASPAutoInit
{
public:
	ASPAutoInit() { ASPInitialize(); }
	~ASPAutoInit() { ASPCleanup(); }
};
static ASPAutoInit aspAutoInit;	

int main()
{
	ASPConnection con;
	try
	{
		con.Open("mybk106", "test123", "test123");
		cout << "�����ݿ�ɹ�" << endl;
	}
	catch (std::exception &ex)
	{
		cout << "�����ݿ�ʧ��:" << ex.what() << endl;
		return 0;
	}

	if (con.IsConnected())
	{
		cout << "���ݿ�������\n";
	}
	else
	{
		cout << "���ݿ��ѶϿ�\n";
	}

	cout << "���ݿ�ϵͳ��ǰʱ��:" << con.SelectOne<ASPDate>("select sysdate from dual").ToString() << '\n';

	con.Close();		// ���Բ�����ʽ����, ASPConnection�����������Զ�����
	cout << "�ر����ݿ�" << endl;

	return 0;
}

