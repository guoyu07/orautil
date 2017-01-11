// asppro.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include <orautil/orautil.hpp>
#include <iostream>
#include <assert.h>
using namespace std;
using namespace asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// SelectOne()��ʹ��

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
		cout << "�����ݿ�ɹ�" << '\n';
	}
	catch (std::exception &ex)
	{
		cout << "�����ݿ�ʧ��:" << ex.what() << '\n';
		return 0;
	}

	try
	{
		short short_max = con.SelectOne<short>("select 32767 from dual");
		assert(short_max == 32767);

		short short_min = con.SelectOne<short>("select -32767 from dual");
		assert(short_min == -32767);

		unsigned short ushort_max = con.SelectOne<unsigned short>("select 65535 from dual");
		assert(ushort_max == 65535);

		unsigned short ushort_min = con.SelectOne<unsigned short>("select 0 from dual");
		assert(ushort_min == 0);

		int int_max = con.SelectOne<int>("select 2147483647 from dual");
		assert(int_max == 2147483647);

		int int_min = con.SelectOne<int>("select -2147483647 from dual");
		assert(int_min == -2147483647);

		unsigned int uint_max = con.SelectOne<unsigned int>("select 4294967295 from dual");
		assert(uint_max == 4294967295);

		unsigned int uint_min = con.SelectOne<unsigned int>("select 0 from dual");
		assert(uint_min == 0);

		float myfloat = con.SelectOne<float>("select 3.14 from dual");
		assert(myfloat == 3.14f);

		double mydouble = con.SelectOne<double>("select 3.1415926 from dual");
		assert(mydouble == 3.1415926);

		string str = con.SelectOne<string>("select 'test string' from dual");
		assert(str == string("test string"));

		ASPDate curDate = con.SelectOne<ASPDate>("select sysdate from dual");
		cout << "DTDate curDate:" << curDate.ToString("yyyy-mm-dd hh24:mi:ss") << '\n';

		// MaxVal(long long) == 9223372036854775807
		long long llmax = con.SelectOne<long long>("select 9223372036854775807 from dual");	
		assert(llmax == 9223372036854775807);

		long long llmin = con.SelectOne<long long>("select -9223372036854775807 from dual");
		assert(llmin == -9223372036854775807);

		// MaxVal(unsigned long long) = 18446744073709551615
		unsigned long long ullmax = con.SelectOne<unsigned long long>("select 18446744073709551615 from dual");	
		assert(ullmax == 18446744073709551615);

		unsigned long long ullmin = con.SelectOne<unsigned long long>("select 0 from dual");
		assert(ullmin == 0);

		int record_count = con.SelectOne<int>("select count(*) from b_kkqp_jgcl");
		cout << "record_count:" << record_count << '\n';

		cout << "����ֵ��ѯ�������\n";
	}
	catch (std::exception &ex)
	{
		cout << "���Գ���:" << ex.what() << '\n';
	}

	con.Close();

	return 0;
}

