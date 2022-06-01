#include "DataBase.h"


template<class ... Args>
inline void DataBase::ExecuteQuery(wstring_view query, function<void()> f, Args*... targets)
{
	SQLRETURN retcode = SQLExecDirect(Stmt_, (SQLWCHAR*)query.data(), SQL_NTS);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		retcode = SQLBindColAutoType(targets...);

		for (;;)
		{
			retcode = SQLFetch(Stmt_);
			if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
				HandleDiagnosticRecord(Stmt_, SQL_HANDLE_STMT, retcode);
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
				f();
			else break;
		}

		SQLCancel(Stmt_);
	}
}



/*========================================
/
/
/		SQLBindColAutoType 가변인자 파싱 처리
/
/
==========================================*/
template<class SQL_t, class ...Args>
inline SQLRETURN DataBase::SQLBindColAutoType(SQLUSMALLINT col, SQL_t* target, Args* ...targets)
{
	SQLBindColAutoType(col, target);
	return SQLBindColAutoType(++col, targets...);
}

template<class ...Args>
inline SQLRETURN DataBase::SQLBindColAutoType(Args* ...targets)
{
	return SQLBindColAutoType(1, targets...);
}


/*========================================
/
/
/		SQLBindColAutoType 특수화
/
/
==========================================*/
template<> inline SQLRETURN DataBase::SQLBindColAutoType(SQLUSMALLINT col, SQLINTEGER* target)
{
	SQLLEN cbTemp{}, lenTemp = 50;
	return SQLBindCol(Stmt_, col, SQL_C_LONG, target, lenTemp, &cbTemp);
}

template<> inline SQLRETURN DataBase::SQLBindColAutoType(SQLUSMALLINT col, SQLWCHAR* target)
{
	SQLLEN cbTemp{}, lenTemp = 50;
	return SQLBindCol(Stmt_, col, SQL_C_WCHAR, target, lenTemp, &cbTemp);
}


/*========================================
/
/
/		SQLBindColAutoType with Any
/
/
==========================================*/

template<> inline SQLRETURN DataBase::SQLBindColAutoType(vector<any>* targets)
{
	SQLRETURN ret{};
	int col = 1;
	for (auto& t : *targets)
	{
		ret = SQLBindColAnyType(col++, &t);
	}
	return ret;
}

inline SQLRETURN DataBase::SQLBindColAnyType(SQLUSMALLINT col, any target)
{
	SQLLEN cbTemp{}, lenTemp = 50;
	SQLRETURN ret{};
	auto targetTypeCode = target.type().hash_code();
	static const auto INTEGER_TYPECODE = typeid(SQLINTEGER).hash_code();
	static const auto WCHAR_TYPECODE = typeid(SQLWCHAR).hash_code();

	if (WCHAR_TYPECODE == targetTypeCode)
	{
		ret = SQLBindColAutoType(col, any_cast<SQLWCHAR*>(target));
	}
	else if (INTEGER_TYPECODE == targetTypeCode)
	{
		ret = SQLBindColAutoType(col, any_cast<SQLINTEGER*>(target));
	}
	else
	{
		cerr << "[ERR] DB::SQLBindColAnyType::target type error." << endl;
	}
	return ret;
}


