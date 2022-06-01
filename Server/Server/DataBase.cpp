#include "stdafx.h"
#include "DataBase.h"
#include "Server.h"

DataBase::DataBase()
{
	SQLRETURN retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &Env_);

	// Set the ODBC version environment attribute  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(Env_, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

		// Allocate connection handle  
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, Env_, &Dbc_);

			// Set login timeout to 5 seconds  
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(Dbc_, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

				// Connect to data source  
				retcode = SQLConnect(Dbc_, (SQLTCHAR*)ODBC_NAME, SQL_NTS, (SQLTCHAR*)NULL, 0, NULL, 0);

				// Allocate statement handle  
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, Dbc_, &Stmt_);
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
					{
						cerr << "DataBase Connect Success" << endl;
						return;
					}
					else
					{
						HandleDiagnosticRecord(Stmt_, SQL_HANDLE_STMT, retcode);
					}
				}
				else HandleDiagnosticRecord(Dbc_, SQL_HANDLE_DBC, retcode);
			}
			else HandleDiagnosticRecord(Dbc_, SQL_HANDLE_DBC, retcode);
		}
		else HandleDiagnosticRecord(Env_, SQL_HANDLE_ENV, retcode);
	}
	else HandleDiagnosticRecord(Env_, SQL_HANDLE_ENV, retcode);

	cerr << "DataBase Connect Fail" << endl;
}

DataBase::~DataBase()
{
	if (Stmt_) SQLFreeHandle(SQL_HANDLE_STMT, Stmt_);
	if (Dbc_)
	{
		SQLDisconnect(Dbc_);
		SQLFreeHandle(SQL_HANDLE_DBC, Dbc_);
	}
	if (Env_) SQLFreeHandle(SQL_HANDLE_ENV, Env_);
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

void DataBase::ProcessQueryQueueLoop()
{
	HANDLE iocp = Server::Get().GetIocp();
	while (true)
	{
		this_thread::sleep_for(1ms);
		QueryRequest e;
		while (QueryRequestQueue_.try_pop(e))
		{
			cout << "!!!" << endl;
		
			ExecuteQuery(e.Query, [&iocp, e]()
				{
				//	auto exover = new DBEventExpOverlapped{ e.Func, e.Targets };
				//	PostQueuedCompletionStatus(iocp, 0, 0, &exover->Over);
					e.Func(*e.Targets);
				}, e.Targets);

//			e.Func(e.Targets);
			cout << ":::" << endl;
		}
	}
}
