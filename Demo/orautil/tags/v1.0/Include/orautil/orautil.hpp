/*!
 * \date 2016/12/01
 * \author ������
 * Contact: liuhaifeng@asppro.com.cn
 *
 * \brief ����ocilib�Ķ��η�װ��
*/

#pragma once

#include <string>
#include <memory>
#include "detail/ocilib/ocilib.hpp"
#include "detail/dataset.hpp"

namespace asppro {
namespace oracle {

	using std::string;

	typedef short					DTShort;
	typedef unsigned short			DTUShort;
	typedef int						DTInt;		// ��Ӧ���ݿ� Integer Type
	typedef unsigned int			DTUInt;
	typedef string					DTStr;		// ��Ӧ���ݿ� VCHAR, NVCHAR Type
	typedef ocilib::Date			DTDate;		// ��Ӧ���ݿ� Date Type
	typedef float					DTFloat;
	typedef double					DTDouble;	// ��Ӧ���ݿ� Float Type
	typedef long long				DTBigInt;	// ��Ӧ���ݿ� big_int
	typedef unsigned long long		DTBigUInt;	// ��Ӧ���ݿ� big_uint

	//
	// Oralce���ݿ������װ��
	//
	// ���߳�:
	//   ����֧�ֶ��߳��µ����ݿ����
	//   �ڶ��߳���ʹ�ô���, ע�����OpenDB(xx, minSize, maxSize)ʱ, ���ݶ��߳�����
	//   ȷ�������minSize(���ٴ򿪵�������), maxSize(����ܹ��򿪵�������), �ܹ�����
	//   ���߳������ݿ��������
	//
	// ���ݿ��ж�����:
	//   ����֧�����ݿ��쳣�жϺ���Զ�����(���, ����Ҫ�ṩ ReConnect() ����)
	//   ʾ��:
		/*try
		{
			Select(sql);
		}
		catch (std::exception &ex)
		{
			switch (ex.GetType().GetValue() == "���ݿ������ж�")
			{
				cout << "���ݿ������ж�, �ȴ����ݿ������ɹ�..." << endl;
				// �ȴ����ݿ������ɹ�
				while (true)
				{
					Thread::Sleep(sometime);
					if (IsConnected())
					{
						cout << "���ݿ������ɹ�" << endl;
						break;
					}
				}
			}
		}*/
	//
	// �쳣����:
	//   ʾ��:
		/*try
		{
			OpenDB();
			Select(sql);
			Execute(sql);
			ExecuteScalar(sql);
			Get<>(idx);
			Execute(sql);
			ExecuteTransaction(sql);
			RollBakc();
			Commit();
		}
		catch (Exception &ex)
		{
			switch (ex.GetType().GetValue())
			{
			case Exception::OracleError:
				cout << text("Oracle Error => ");
				break;
			case Exception::OracleWarning:
				cout << text("Oracle Error => ");
				break;
			case Exception::OcilibError:
				cout << text("OCILIB Error => ");
				break;
			default:
				cout << text("Unknown Error => ");
				break;
			}

			cout << ex.what() << endl;
		}
		catch (std::exception& ex)
		{
			cout << ex.what() << endl;
		}*/

	class OracleUtil
	{
	public:
		OracleUtil();
		~OracleUtil();

		// �����ݿ�(������ָ������������)
		//	 db:		���ݿ������
		//   user:		�û���
		//   pwd:		����
		//   minSize:	���ٴ򿪵�������     assert(minSize > 0)
		//   maxSize:	�����Դ򿪵������� assert(maxSize > minSize)
		static void OpenDB(const string& db, const string& user, 
			const string& pwd, unsigned int minSize = 1, 
			unsigned int maxSize = 2);

		// �ر����ݿ�(�����д򿪵�����)
		static void CloseDB() throw();

		// ���ݿ������Ƿ���Ч
		static bool IsConnected() throw();

		// ���ݲ�ѯ
		// ʾ��:
		/*Select("select str, int, date, double from xxx");
		while (Next())
		{
			try
			{
				string str = Get<DTStr>(1);
				int num = Get<DTInt>(2);
				time_t date = Get<DTDate>(3);
				double dbl = Get<DTDouble>(4);
			}
			catch(std::exception& ex)
			{
				cout << "�ֶ����ݻ�ȡ�������ֵ����ת������:" << ex.what() << endl;
			}
		}*/
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		void Select(const string& sql);

		// ���ݿ��ѯ�����λ����һ��
		// ע: ִ��Select()��, ���ص����ݼ���һ��Ϊ"������", "������"�Ժ��������
		//     ����ִ��Select()�������һ��Next()����"������", Ȼ���ٵ���Get<>()
		//     ȡ����
		inline bool Next() throw();

		// ע: index �� 1 ��ʼ
		template <typename DbType>
		DbType Get(int index);
		// ע: index ��1 ��ʼ
		template <>
		DTDate Get<DTDate>(int index);
		// ----------------------------------------------------------------------

		// ���ݲ�ѯ: ���ؼ���
		// ���ݲ�ѯ
		// ʾ��:
		/*
		std::unique_ptr<Dataset> dt = Select2("select str, int, date, double from xxx");
		while (dt->Next())
		{
			try
			{
				string str = dt->Get<DTStr>(1);
				int num = dt->Get<DTInt>(2);
				time_t date = dt->Get<DTDate>(3);
				double dbl = dt->Get<DTDouble>(4);
			}
			catch(std::exception& ex)
			{
				cout << "�ֶ����ݻ�ȡ�������ֵ����ת������:" << ex.what() << endl;
			}
		}*/
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		std::unique_ptr<Dataset> Select2(const string& sql);
		// ----------------------------------------------------------------------

		// ��ѯ����ֵ (��������ݿ��ѯ��һֵ)
		// ʾ��:
		// cout << "sysdate:" << ExecuteScalar<DTDate>("select sysdate from dual");
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		template <typename DbType>
		DbType SelectOne(const string& sql);
		template <>
		DTDate SelectOne<DTDate>(const string& sql);
		// ----------------------------------------------------------------------

		// ���롢ɾ�������� (�Զ��ύģʽ)
		// ����ֵ: ��Ӱ�������
		int Execute(const string& sql);

		// ���롢ɾ�������� (����ģʽ)
		// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		int ExecuteTransaction(const string& sql);
		inline void Rollback();		// �ع�
		inline void Commit();		// �ύ
		// -----------------------------------------------------------------------

	private:
		inline void CloseOpenConSel();
		inline void CloseOpenConTrac();

		static ocilib::Pool		m_Pool;
		ocilib::Connection		m_ConSel;
		ocilib::Connection		m_ConTrac;
		ocilib::Statement		m_St;
		ocilib::Resultset*		m_pRs;
	};

	// ���ߺ���
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// time_t ת string
	string Time2Str(const time_t& time, const string& format = "yyyy-mm-dd hh24:mi:ss");

	// string ת time_t
	time_t Str2Time(const string& timeStr, const string& format = "yyyy-mm-dd hh24:mi:ss");

	// ocilib::Date ת time_t
	time_t Date2Time(const ocilib::Date& date);

	// time_t ת ocilib::Date
	ocilib::Date Time2Date(const time_t& time);
	// -----------------------------------------------------------------------------
} // namesapce oracle
} // namespace asppro

#include "detail/orautil_iml.hpp"
