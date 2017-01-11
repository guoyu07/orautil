#ifndef ASPPRO_ORACLE_ORAUTIL_H_
#define ASPPRO_ORACLE_ORAUTIL_H_

#pragma once

#include "detail/ocilib/ocilib.hpp"
#include <memory>

namespace asppro {
namespace oracle {
	
// ****************************************************************************** 
// ��װ�⻷����ʼ����ע��
// ע: 1. ������, ��ʹ��asppro::oracle�����ڵ���������(��: ASPPool, ASPConnection, 
//     ASPDataset, ASPDate)֮ǰ, ���ȷ���⻷����ʼ��, ����ʹ��������������, ����
//     ���ؿ�ֵ
//     2. ��װ�⻷����ʼ����ע��Ӧ�ó�����ֻ��һ�μ���, ��γ�ʼ���������������.
// ****************************************************************************** 
// ����RAII�ַ�ʵ��"�Զ���ʼ��" ʾ��: 
class OciEnvInit
{
	public:
		OciEnvInit(ocilib::Environment::EnvironmentFlags mode 
			= ocilib::Environment::Default | ocilib::Environment::Threaded, 
			bool enableWarnings = true,
			const ocilib::ostring& libpath = "") 
		{
			// �⻷����ʼ��(�������ȫ�ֹ�����Դ)
			ocilib::Environment::Initialize(mode, libpath);
			ocilib::Environment::EnableWarnings(enableWarnings);
		}

		~OciEnvInit() 
		{
			// �⻷��ע��(ʹ�÷����ȫ�ֹ�����Դ)
			ocilib::Environment::Cleanup();
		}
};

// ****************************************************************************** 
// ��ѯ���ݿ��Ƿ�������״̬
// ****************************************************************************** 
bool IsConnected(const ocilib::Connection& con);

// ****************************************************************************** 
// ��ֵ��ѯ
// ****************************************************************************** 
template<typename TDataType>
TDataType SelectOne(const ocilib::Connection& con, const ocilib::ostring& sql);

// ****************************************************************************** 
// ocilib::Date �����/��  (������, ������)
// ****************************************************************************** 
void AddHours(ocilib::Date& dt, int hours);
void AddMintues(ocilib::Date& dt, int mintues);
void AddSeconds(ocilib::Date& dt, int seconds);

// ****************************************************************************** 
// ���ڹ��ߺ���
// ****************************************************************************** 
ocilib::Date Time2Date(const time_t& tt);
time_t Date2Time(const ocilib::Date& dt);

time_t Str2Time(const ocilib::ostring& timeStr, 
	const ocilib::ostring& format = OTEXT("yyyy-mm-dd hh24:mi:ss"));	
ocilib::ostring Time2Str(const time_t& time, 
	const ocilib::ostring& format = OTEXT("yyyy-mm-dd hh24:mi:ss"));

} // oracle 
} // asppro

#include "detail/orautil_iml.hpp"

#endif // ASPPRO_ORACLE_ORAUTIL_H_
