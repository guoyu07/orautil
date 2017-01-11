// asppro.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include <orautil/orautil.hpp>
#include <iostream>
#include <assert.h>
using namespace std;
using namespace asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// Select()��ʹ��

int main()
{
	try
	{
		//OracleUtil::OpenDB("���ݿ������", "�û���", "����");
		OracleUtil::OpenDB("mybk106", "bsitsyx2", "bsitsyx2");
		cout << "�����ݿ�ɹ�" << endl;
	}
	catch (std::exception &ex)
	{
		cout << "�����ݿ�ʧ��:" << ex.what() << endl;
		return 0;
	}

	if (OracleUtil::IsConnected())
	{
		cout << "���ݿ�������" << endl;
	}
	else
	{
		cout << "���ݿ��ѶϿ�" << endl;
	}

	cout << "���\t" << "��¼���\t" << "���ƺ���\t" << "����ʱ��\t" << "cwkc\t" << "�豸���\t" << endl;

	OracleUtil ora;
	try
	{
		ora.Select("select jlbh, hphm, jgsj, cwkc, sbbh from b_kkqp_jgcl where rownum <= 30");
	}
	catch (std::exception &ex)
	{
		cout << "��ѯ����:" << ex.what() << endl;
		return 0;
	}

	int idx = 0;
	while (ora.Next())
	{
		try
		{
			string jlbh = ora.Get<DTStr>(1);
			string hphm = ora.Get<DTStr>(2);
			ocilib::Date jgsj = ora.Get<DTDate>(3);
			double cwkc = ora.Get<DTDouble>(4);
			string sbbh = ora.Get<DTStr>(5);

			cout << "[" << ++idx << "]:\t" << jlbh << ",\t" << hphm << ",\t" 
				<< jgsj.ToString("yyyy-mm-mm hh24:mi:ss") << ",\t"
				<< cwkc << ",\t" << sbbh << endl;
		}
		catch (std::exception &ex)
		{
			cout << "��ȡ�ֶγ���:" << ex.what() << endl;
			break;
		}
	}

	return 0;
}

