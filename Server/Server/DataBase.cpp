#include "stdafx.h"
#include "DataBase.h"
#include "Server.h"


// example 
/*
{
	QueryRequest q;
	q.Query = L"EXEC SelectCharacterDataGreaterLevel -1"sv;
	q.Targets = make_shared<vector<any>>(); q.Targets->reserve(3);
	q.Targets->emplace_back(make_any<SQLINTEGER>());
	q.Targets->emplace_back(make_any<wstring>());
	q.Targets->emplace_back(make_any<SQLINTEGER>());
	q.Func = [](const vector<any>& t)
	{
		wcout << "[ DB TEST ] " << any_cast<SQLINTEGER>(t[0]) << " :: "
			<< any_cast<wstring>(t[1]) << " :: "
			<< any_cast<SQLINTEGER>(t[2]) << endl;
	};
	DataBase::Get().AddQueryRequest(q);
}
*/
/*
	{
	QueryRequest q;
	q.Query = L"EXEC UserLogin admin,admin"s;
	q.Targets = make_shared<vector<any>>(); q.Targets->reserve(1);
	q.Targets->emplace_back(make_any<SQLWCHAR>());
	//q.Targets->emplace_back(make_any<SQLWCHAR>());
	q.Func = [](const vector<any>& t)
	{
		wcout << "[ DB TEST !! ] " << any_cast<SQLWCHAR>(t[0]) << " :: " << endl;
	};
	DataBase::Get().AddQueryRequest(q);
	}
*/



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
				SQLSetConnectAttr(Dbc_, SQL_LOGIN_TIMEOUT, (SQLPOINTER)10, 0);

				// Connect to data source  
				retcode = SQLConnect(Dbc_, (SQLTCHAR*)ODBC_NAME, SQL_NTS, (SQLTCHAR*)NULL, 0, NULL, 0);

				// Allocate statement handle  
				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, Dbc_, &Stmt_);
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
					{
						cerr << "DataBase Connection Success" << endl;
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

	cerr << "DataBase Connection Fail" << endl;
	exit(-1);
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

void DataBase::HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE& RetCode, wstring_view mess)
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
		if (!wcsncmp(wszState, L"24000", 5)) {
			RetCode = SQL_NORETURN;
			continue;
		}

		wcout << mess << endl << wszState << " " << wszMessage << " (" << iError << ")" << endl;
	}
}

void DataBase::ProcessQueryQueueLoop()
{
	HANDLE iocp = Server::Get().GetIocp();
	while (true)
	{
		this_thread::sleep_for(1ms);
		thread_local QueryRequest e;
		while (QueryRequestQueue_.try_pop(e))
		{
			ExecuteQuery(e.Query, [&iocp, f = e.Func, r = e.Targets]()
			{
				//e.Func(*e.Targets);
				auto exover = new DataBaseExpOverlapped{ f, *r };
				PostQueuedCompletionStatus(iocp, 0, 0, &exover->Over);
			}, * e.Targets);
		}
	}
}
