#ifndef ASPPRO_ORACLE_ORAUTIL_H_
#define ASPPRO_ORACLE_ORAUTIL_H_

#pragma once

#include <string>
#include <vector>
#include "detail/ocilib/ocilib.hpp"

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
/*class ASPAutoInit
{
	public:
		ASPAutoInit() { asppro::oracle::Environment::ASPInitialize(); }
		~ASPAutoInit() { asppro::oracle::Environment::ASPCleanup(); }
};

static ASPAutoInit aspAutoInit;	
*/
void  ASPInitialize();		// �⻷����ʼ��(�������ȫ�ֹ�����Դ)
void  ASPCleanup();			// �⻷��ע��(ʹ�÷����ȫ�ֹ�����Դ)


// ****************************************************************************** 
// ��װ��������
// ******************************************************************************
class ASPDate
{
	typedef enum { NullDate } NullDateType;		// ����������

public:
	// ASPDate()Ĭ�Ϲ���Ϊ"������", ����ʹ��IsNull()��ѯ״̬, 
	// "������"��, ���ó�IsNull()֮���������Ա���������׳��쳣
	ASPDate(NullDateType dt = NullDate);	

	// ʹ��time_t����
	explicit ASPDate(const time_t& tt);

	// �ַ���ʽ���ڹ���
	// ע: ASPDate���ַ������ڸ�ʽ������ѭ"Oracle���ݿ�Data��ʽ����", 
	//    ����"��Сд����"
	//    ��:		yyyy
	//    ��:		mm
	//    ��:		dd
	//    24Сʱ:   hh24
	//	  ����:		mi
	//    ��:		ms
	//    ʾ��: yyyy-mm-dd hh24:mi:ss  �� yyyy/mm/dd hh24:mi:ss
	explicit ASPDate(const std::string& str, const std::string& format = "yyyy-mm-dd hh24:mi:ss");
	ASPDate(const ASPDate& dt);
	~ASPDate(void);

	// ��ѯ�Ƿ�Ϊ"������"
	//   ����ѯ�����ݿ�Date�ֶ�Ϊ��ʱ, ���ص�ASPDataΪ"������"
	bool	IsNull()	const;

	// ʹ�������ַ�����ʼ������
	void FromString(const std::string& str, const std::string& format = "yyyy-mm-dd hh24:mi:ss");

	// ʹ��time_t��ʼ������
	void FromTimet(const time_t& tt);

	std::string ToString( const std::string& format = "yyyy-mm-dd hh24:mi:ss") const;
	time_t GetTimet() const;

	ASPDate& operator=(const ASPDate& dt);

	ASPDate&	operator++();						// by second				
	ASPDate		operator++(int);					// by second
	ASPDate&	operator--();						// by second
	ASPDate		operator--(int);						// by second
	ASPDate		operator+(int seconds);				// by seconds
	ASPDate		operator-(int seonds);				// by seconds
	ASPDate&	operator+= (int seconds);			// by seconds
	ASPDate&	operator-= (int seconds);			// by seconds

	bool operator == (const ASPDate& dt)	const;
	bool operator != (const ASPDate& dt)	const;
	bool operator >  (const ASPDate& dt)	const;
	bool operator <  (const ASPDate& dt)	const;
	bool operator >= (const ASPDate& dt)	const;
	bool operator <= (const ASPDate& dt)	const;

	// static function
	// ++++++++++++++++++++++
	static ASPDate SysDate();
	static time_t Str2Time(const std::string& timeStr, const std::string& format = "yyyy-mm-dd hh24:mi:ss");	
	static std::string Time2Str(const time_t& time, const std::string& format = "yyyy-mm-dd hh24:mi:ss");
	// ----------------------
private:
	static const std::string& ASPDate::DefFmt();

	time_t m_tt;
};


// ****************************************************************************** 
// ��װ��Dataset��
// �������ݿ�������ѯ���
// ******************************************************************************
class ASPConnection;
class  ASPDataset
{
	friend class ASPConnection;

public:
	ASPDataset();
	ASPDataset(const ASPDataset& dt);
	ASPDataset& operator=(const ASPDataset& dt);
	~ASPDataset(void);

	// ָ���ѯ���ϵ���һ��
	// 
	// ���ݲ�ѯ��ȡ��ASPDataset��, ��һ(��: index = 0)��Ϊ������,
	// ��Ҫȡ����֮ǰ, ����Ҫ�ȵ���ASPDataset.Next(), ��ȡ����.
	// ȡ��һ������, �����ε���Next()ָ����һ��, ���ν���
	// ��ָ��������β, Next()����false
	bool Next();

	// ����index��ȡ��ǰ���ֶ�, index �� 1 ��ʼ
	//
	// ע: ���ݿ��е�һ(��: index = 0)��, ΪOracle ��RowID
	// �������ֶβ�ѯindex �� 1 ��ʼ
	template <typename TDataType>
	TDataType Get(unsigned int index);

	// ����������ȡ��ǰ���ֶ�
	template <typename TDataType>
	TDataType Get(const std::string& fieldName);

private:
	ASPDataset(const ASPConnection& con, const std::string& sql);
	void Release();

	ocilib::Connection* m_pCon;
	ocilib::Statement* m_pSt;
	ocilib::Resultset* m_pRs;
};


// ****************************************************************************** 
// ��װ��Connection��
// ******************************************************************************
class  ASPConnection
{
	friend class ASPPool;
	friend class ASPDataset;

public:
	ASPConnection(void);

	// �����ݿ�
	// dbServiceName: ���ݿ������
	// user:		  �û���
	// pwd:			  ����
	ASPConnection(const std::string& dbServiceName, const std::string& user, 
		const std::string& pwd);
	ASPConnection(const ASPConnection& con);
	ASPConnection& operator=(const ASPConnection& con);

	// �����ǰ���ݿ�Ϊ��״̬, �����������л��Զ��ر�
	~ASPConnection(void);

	// �����ݿ�
	// dbServiceName: ���ݿ������
	// user:		  �û���
	// pwd:			  ����
	void Open(const std::string& dbServiceName, const std::string& user, 
	const std::string& pwd);

	// �ر����ݿ�
	void Close();

	// �ύ���ݿ����
	void Commit();

	// �ع����ݿ����
	void Rollback();

	// �жϵ�ǰ��ѯ
	void Break();

	// �������ݿ��Զ��ύģʽ (ASPConnection Ĭ�� GetAutoCommit() == false)
	void SetAutoCommit(bool enabled);

	// ��ѯ��ǰ���ݿ��Զ��ύģʽ
	bool GetAutoCommit() const;

	// ��ѯ���ݿ��Ƿ�������״̬
	bool IsConnected() const;

	// ���ݿⵥ��ֵ��ѯ
	template<typename TDataType>
	TDataType SelectOne(const std::string& sql) const;

	// ���ݿ��������ݲ�ѯ
	ASPDataset Select(const std::string& sql);

	// ���ݿ���Ĳ���
	void Execute(const std::string& sql);

	// ���ݿ��������Ĳ���
	void Execute(const std::vector<std::string>& sqlList);

private:
	ocilib::Connection* m_pCon;

	ASPConnection(const ocilib::Connection& con);
	void Release();
};


// ****************************************************************************** 
// ��װ�����ӳ�Pool��
// ******************************************************************************
class  ASPPool
{
public:
	ASPPool(void);

	// �����ӳ�
	// dbServiceName:		���ݿ������
	// user:				�û���
	// pwd:					����
	// minSize:				���ӳ���С������  
	// maxSize:				���ӳ����������  assert(maxSize > minSize)
	// increment:			���ӳ�������
	ASPPool(const std::string& dbServiceName, const std::string& user,
		const std::string& pwd, unsigned int minSize, unsigned int maxSize,
		unsigned int increment = 1);

	// �����������Զ��رմ򿪵����ӳ�
	~ASPPool(void);

	// �����ӳ�
	// dbServiceName:		���ݿ������
	// user:				�û���
	// pwd:					����
	// minSize:				���ӳ���С������  
	// maxSize:				���ӳ����������  assert(maxSize > minSize)
	// increment:			���ӳ�������
	void Open(const std::string& dbServiceName, const std::string& user,
		const std::string& pwd, unsigned int minSize, unsigned int maxSize,
		unsigned int increment = 1);

	// �ر����ӳ�
	void Close();

	// �����ӳػ�ȡһ����������
	ASPConnection GetConnection();

private:
	ASPPool(const ASPPool&);
	ASPPool& operator=(const ASPPool&);
	void Release();

	ocilib::Pool* m_pPool;
};

} // oracle 
} // asppro

#include "detail/orautil_iml.hpp"

#endif // ASPPRO_ORACLE_ORAUTIL_H_
