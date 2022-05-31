

template<class ... Args>
inline void DataBase::ExecuteQuery(wstring_view query, function<void()> f, Args*... targets)
{
	SQLRETURN retcode = SQLExecDirect(hstmt_, (SQLWCHAR*)query.data(), SQL_NTS);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		retcode = SQLBindColAutoType(targets...);

		// Fetch and print each row of data. On an error, display a message and exit.  
		for (;;)
		{
			retcode = SQLFetch(hstmt_);
			if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
				HandleDiagnosticRecord(hstmt_, SQL_HANDLE_STMT, retcode);
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				f();
			}
			else
				break;
		}

		SQLCancel(hstmt_);
	}
}




template<class SQL_t, class ...Args>
SQLRETURN SQLBindColAutoType(SQLUSMALLINT col, SQL_t* target, Args*... targets)
{
	SQLBindColAutoType(col, target);
	return SQLBindColAutoType(++col, targets...);
}

template<> inline SQLRETURN DataBase::SQLBindColAutoType(SQLUSMALLINT col, SQLINTEGER* target)
{
	SQLLEN cbTemp{}, lenTemp = 50;
	return SQLBindCol(hstmt_, col++, SQL_C_LONG, target, lenTemp, &cbTemp);
}

template<> inline SQLRETURN DataBase::SQLBindColAutoType(SQLUSMALLINT col, SQLWCHAR* target)
{
	SQLLEN cbTemp{}, lenTemp = 50;
	return SQLBindCol(hstmt_, col++, SQL_C_WCHAR, target, lenTemp, &cbTemp);
}