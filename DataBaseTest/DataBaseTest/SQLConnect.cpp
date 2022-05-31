// SQLConnect_ref.cpp  
// compile with: odbc32.lib  
#include <windows.h>  
#include <sqlext.h>  

#include <iostream>
using namespace std;

int main() {
    SQLHENV henv;       // 환경 핸들
    SQLHDBC hdbc;       // DB 핸들
    SQLHSTMT hstmt;     // 
    SQLRETURN retcode;  // 결과리턴값

    SQLCHAR* OutConnStr = (SQLCHAR*)malloc(255);
    SQLSMALLINT* OutConnStrLen = (SQLSMALLINT*)malloc(255);

    // Allocate environment handle  
    retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

    // Set the ODBC version environment attribute  
    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

        // Allocate connection handle  
        if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
            retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

            // Set login timeout to 5 seconds  
            if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
                SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0); // 5초 동안 시도
                
                constexpr TCHAR OdbcName[] = L"GameServerProgrammingDB";
                // Connect to data source  
                retcode = SQLConnect(hdbc, (SQLTCHAR*)OdbcName, SQL_NTS, (SQLTCHAR*)NULL, 0, NULL, 0);

                // Allocate statement handle  
                if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
                    cout << " SQLConnect Success" << endl;
                    SQLBindCol()
                    retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

                    // Process data  
                    if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
                        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
                    }
                    else
                    {
                        cout << " SQLAllocHandle Fail" << endl;
                    }

                    SQLDisconnect(hdbc);
                }
                else
                {
                    cout << " SQLConnect Fail" << endl;
                }

                SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
            }
            else
            {
                cout << " SQLAllocHandle (HDBC) Fail" << endl;
            }
        }
        else
        {
            cout << " SQLSetEnvAttr Fail" << endl;
        }
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
    }
    else
    {
        cout << " SQLAllocHandle (HENV) Fail" << endl;
    }
}