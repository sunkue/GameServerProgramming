#pragma once


class DataBase
{
	static constexpr TCHAR OdbcName[] = L"GameServerProgrammingDB";
	SINGLE_TON(DataBase);
	~DataBase();
private:
protected:
public:
	template<class ... Args>
	void ExecuteQuery(wstring_view query, function<void()> f, Args*... targets);
private:

	template<class SQL_t, class ...Args>
	SQLRETURN SQLBindColAutoType(SQLUSMALLINT col, SQL_t* target, Args*... targets)
	{
		SQLBindColAutoType(col, target);
		return SQLBindColAutoType(++col, targets...);
	}

	template<class ...Args>
	SQLRETURN SQLBindColAutoType(Args*... targets)
	{
		return SQLBindColAutoType(1, targets...);
	}

	static void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);
protected:
	SQLHENV henv_{};
	SQLHDBC hdbc_{};
	SQLHSTMT hstmt_{};
public:
};

#include "DataBase.hpp"