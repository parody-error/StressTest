#include "pch.h"
#include <windows.h>
#include <sql.h>
#include <sqlext.h>

extern "C" __declspec( dllexport ) int RunQueryStress( const char* dsn, const char* query )
{
    SQLHENV hEnv;
    SQLHDBC hDbc;
    SQLHSTMT hStmt;
    SQLRETURN ret;

    SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv );
    SQLSetEnvAttr( hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0 );
    SQLAllocHandle( SQL_HANDLE_DBC, hEnv, &hDbc );

    ret = SQLConnectA( hDbc, (SQLCHAR*)dsn, SQL_NTS, NULL, 0, NULL, 0 );
    if ( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
        return -1;

    SQLAllocHandle( SQL_HANDLE_STMT, hDbc, &hStmt );
    ret = SQLExecDirectA( hStmt, (SQLCHAR*)query, SQL_NTS );

    SQLFreeHandle( SQL_HANDLE_STMT, hStmt );
    SQLDisconnect( hDbc );
    SQLFreeHandle( SQL_HANDLE_DBC, hDbc );
    SQLFreeHandle( SQL_HANDLE_ENV, hEnv );

    return ( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO ) ? 0 : -2;
}
