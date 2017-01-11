// asppro.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include <orautil/orautil.hpp>
#include <iostream>
#include <assert.h>
using namespace std;
using namespace asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// Select()��ʹ��

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
		con.Open("mybk106", "bsitsyx2", "bsitsyx2");
		cout << "�����ݿ�ɹ�" << '\n';
	}
	catch (std::exception &ex)
	{
		cout << "�����ݿ�ʧ��:" << ex.what() << '\n';
		return 0;
	}

	cout << "���\t��¼���\t���ƺ���\t����ʱ��\tcwkc\t�豸���\t\n";

	ASPDataset dt;
	try
	{
		dt = con.Select("select jlbh, hphm, jgsj, cwkc, sbbh from b_kkqp_jgcl where rownum <= 30");
	}
	catch (std::exception &ex)
	{
		cout << "��ѯ����:" << ex.what() << '\n';
		return 0;
	}

	int idx = 0;
	while (dt.Next())
	{
		try
		{
			string jlbh = dt.Get<string>(1);
			string hphm = dt.Get<string>(2);
			ASPDate jgsj = dt.Get<ASPDate>(3);
			double cwkc = dt.Get<double>(4);
			string sbbh = dt.Get<string>(5);

			cout << "[" << ++idx << "]:\t" << jlbh << ",\t" << hphm << ",\t" 
				<< jgsj.ToString("yyyy-mm-mm hh24:mi:ss") << ",\t"
				<< cwkc << ",\t" << sbbh << '\n';
		}
		catch (std::exception &ex)
		{
			cout << "��ȡ�ֶγ���:" << ex.what() << '\n';
			break;
		}
	}

	con.Close();

	return 0;
}

