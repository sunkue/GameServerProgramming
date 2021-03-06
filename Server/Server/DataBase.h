#pragma once

#define SQL_NORETURN -999

struct QueryRequest
{
	wstring Query;
	function<void(vector<any>)> Func;
	shared_ptr<vector<any>> Targets;
};

class DataBase
{
	static constexpr TCHAR ODBC_NAME[] = L"GameServerProgrammingDB";
	// THREAD_LOCAL_SINGLE_TON(DataBase);
	SINGLE_TON(DataBase);
	~DataBase();
public:
	void AddQueryRequest(QueryRequest&& e) { QueryRequestQueue_.push(e); }
	void AddQueryRequest(QueryRequest& e) { QueryRequestQueue_.push(move(e)); }
protected:
	template<class ... Args>
	void ExecuteQuery(wstring_view query, function<void()> f, Args&... targets);
private:
	template<class SQL_t, class ...Args>
	SQLRETURN SQLBindColAutoType(SQLUSMALLINT col, SQL_t& target, Args&... targets);
	template<class ...Args>
	SQLRETURN SQLBindColAutoType(Args&... targets);
	SQLRETURN SQLBindColAnyType(SQLUSMALLINT col, any& target);
private:
	static void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE& RetCode, wstring_view mess = L""s);
public:
	void ProcessQueryQueueLoop();
private:
	SQLHENV Env_{};
	SQLHDBC Dbc_{};
	SQLHSTMT Stmt_{};
	concurrent_queue<QueryRequest> QueryRequestQueue_;
};


#include "DataBase.hpp"