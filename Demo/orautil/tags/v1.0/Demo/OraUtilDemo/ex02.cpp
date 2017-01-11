// asppro.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include <orautil/orautil.hpp>
#include <iostream>
#include <assert.h>
using namespace std;
using namespace asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// SelectOne()��ʹ��

int main()
{
	try
	{
		//OracleUtil::OpenDB("���ݿ������", "�û���", "����");
		OracleUtil::OpenDB("mybk106", "test123", "test123");
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

	OracleUtil ora;

	try
	{
		short short_max = ora.SelectOne<DTShort>("select 32767 from dual");
		assert(short_max == 32767);

		short short_min = ora.SelectOne<DTShort>("select -32767 from dual");
		assert(short_min == -32767);

		unsigned short ushort_max = ora.SelectOne<DTUShort>("select 65535 from dual");
		assert(ushort_max == 65535);

		unsigned short ushort_min = ora.SelectOne<DTUShort>("select 0 from dual");
		assert(ushort_min == 0);

		int int_max = ora.SelectOne<DTInt>("select 2147483647 from dual");
		assert(int_max == 2147483647);

		int int_min = ora.SelectOne<DTInt>("select -2147483647 from dual");
		assert(int_min == -2147483647);

		unsigned int uint_max = ora.SelectOne<DTUInt>("select 4294967295 from dual");
		assert(uint_max == 4294967295);

		unsigned int uint_min = ora.SelectOne<DTUInt>("select 0 from dual");
		assert(uint_min == 0);

		float myfloat = ora.SelectOne<DTFloat>("select 3.14 from dual");
		assert(myfloat == 3.14f);

		double mydouble = ora.SelectOne<DTDouble>("select 3.1415926 from dual");
		assert(mydouble == 3.1415926);

		string str = ora.SelectOne<DTStr>("select 'test string' from dual");
		assert(str == string("test string"));

		ocilib::Date curDate = ora.SelectOne<DTDate>("select sysdate from dual");
		cout << "DTDate curDate:" << curDate.ToString("yyyy-mm-dd hh24:mi:ss") << endl;

		time_t curTime = Date2Time(ora.SelectOne<DTDate>("select sysdate from dual"));
		cout << "time_t curTime:" << Time2Str(curTime, DTStr("yyyy-mm-dd hh24:mi:ss")) << endl;

		// MaxVal(long long) == 9223372036854775807
		long long llmax = ora.SelectOne<DTBigInt>("select 9223372036854775807 from dual");	
		assert(llmax == 9223372036854775807);

		long long llmin = ora.SelectOne<DTBigInt>("select -9223372036854775807 from dual");
		assert(llmin == -9223372036854775807);

		// MaxVal(unsigned long long) = 18446744073709551615
		unsigned long long ullmax = ora.SelectOne<DTBigUInt>("select 18446744073709551615 from dual");	
		assert(ullmax == 18446744073709551615);

		unsigned long long ullmin = ora.SelectOne<DTBigUInt>("select 0 from dual");
		assert(ullmin == 0);

		int record_count = ora.SelectOne<DTInt>("select count(*) from b_kkqp_jgcl");
		cout << "record_count:" << record_count << endl;

		cout << "����ֵ��ѯ�������" << endl;
	}
	catch (std::exception &ex)
	{
		cout << "���Գ���:" << ex.what() << endl;
	}

	return 0;
}

