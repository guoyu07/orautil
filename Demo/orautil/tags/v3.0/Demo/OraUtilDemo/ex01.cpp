// asppro::oracle.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include <orautil/orautil.hpp>
#include <iostream>
namespace asp = asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// ���ݿ⻷����ʼ����ע��
// ע: �κβ������ݿ�֮ǰ, ����Ҫ"��ʼ�����ݿ⻷��", ����ʹ�����Ҫ"������ݿ⻷��������Դ"
//     ������Com�ĳ�ʼ����ע��, "���ݿ⻷����ʼ����ע��"��ȫ�ֵ�, һ��Ӧ�ó�����ֻ��һ�μ���
static asp::OciEnvInit ociEnvInit;

int main()
{
	// ���ݿ���ز���
	// ...
	// ...

	// ���ocilib�⻷����ʼ���ɹ�, ��ɴ����ݿ��ѯ����ȷϵͳʱ��, ���򷵻�NullData, �����쳣
	ocilib::Connection con;
	try
	{
		con.Open("mybk106", "test123", "test123");
		std::cout << asp::SelectOne<ocilib::Date>(con, "select sysdate from dual").ToString("yyyy-mm-dd hh24:mi:ss") << '\n';
		std::cout << "ocilib�⻷����ʼ���ɹ�\n";
	}
	catch (std::exception &ex)
	{
		std::cout << "�쳣����:" << ex.what() << '\n';
	}

	return 0;
}

