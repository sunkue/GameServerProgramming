#include "stdafx.h"
#include "DataBase.h"

DataBase::DataBase()
{
	SQLRETURN retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv_);

	// Set the ODBC version environment attribute  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv_, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

		// Allocate connection handle  
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv_, &hdbc_);

			// Set login timeout to 5 seconds  
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc_, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

				// Connect to data source  
				retcode = SQLConnect(hdbc_, (SQLTCHAR*)OdbcName, SQL_NTS, (SQLTCHAR*)NULL, 0, NULL, 0);

				// Allocate statement handle  
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc_, &hstmt_);
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) 
					{
						cerr << "DataBase Connect Success" << endl;
						return;
					}
					else
					{
						HandleDiagnosticRecord(hstmt_, SQL_HANDLE_STMT, retcode);
					}
				}
				else HandleDiagnosticRecord(hdbc_, SQL_HANDLE_DBC, retcode);
			}
			else HandleDiagnosticRecord(hdbc_, SQL_HANDLE_DBC, retcode);
		}
		else HandleDiagnosticRecord(henv_, SQL_HANDLE_ENV, retcode);
	}
	else HandleDiagnosticRecord(henv_, SQL_HANDLE_ENV, retcode);

	cerr << "DataBase Connect Fail" << endl;
}

DataBase::~DataBase()
{
	if (hstmt_) SQLFreeHandle(SQL_HANDLE_STMT, hstmt_);
	if (hdbc_)
	{
		SQLDisconnect(hdbc_);
		SQLFreeHandle(SQL_HANDLE_DBC, hdbc_);
	}
	if (henv_) SQLFreeHandle(SQL_HANDLE_ENV, henv_);
}

void DataBase::HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
{
	std::wcout.imbue(std::locale("korean"));
	SQLSMALLINT iRec = 0;
	SQLINTEGER iError;
	WCHAR wszMessage[1000];
	WCHAR wszState[SQL_SQLSTATE_SIZE + 1];
	if (SQL_INVALID_HANDLE == RetCode)
	{
		wcerr << "Invalid handle!" << endl;
		return;
	}
	while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage,
		(SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT*)NULL) == SQL_SUCCESS)
	{
		if (wcsncmp(wszState, L"01004", 5)) {
			wcout << wszState << " " << wszMessage << " (" << iError << ")" << endl;
		}
	}
}
