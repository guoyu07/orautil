// asppro::oracle.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include <orautil/orautil.hpp>
#include <iostream>
namespace asp = asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// ��, �ر����ݿ� ��ʾ

static asp::OciEnvInit ociEnvInit;

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

	if (asp::IsConnected(con))
	{
		std::cout << "���ݿ�������\n";
	}
	else
	{
		std::cout << "���ݿ��ѶϿ�\n";
	}

	std::cout << "���ݿ�ϵͳ��ǰʱ��:" << asp::SelectOne<ocilib::Date>(con, "select sysdate from dual").ToString() << '\n';

	std::cout << "�ر����ݿ�\n";

	con.Close();

	return 0;
}

