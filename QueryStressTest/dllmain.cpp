#include "pch.h"
#include "OdStatement.h"
#include "OdResultset.h"
#include "IColumn.h"

#include <atlstr.h>
#include <DbgHelp.h>
#include <set>
#include <sql.h>
#include <sqlext.h>
#include <string>
#include <tchar.h>
#include <windows.h>

#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "odbc32.lib")

const int UNKNOWN_RESULT = -9;
const int SUCCESS_RESULT = 0;
const int AUTH_ERROR = -1;
const int USE_DATABASE_ERROR = -2;
const int QUERY_ERROR = -3;
const int ODBC_ERROR = -4;
const int DESCRIBE_ERROR = -5;
const int NO_TABLES_ERROR = -6;
const int EXCEPTION_DESCRIBING_TABLES = -7;

const int SCHEMA_NAME_COLUMN = 1;
const int TABLE_NAME_COLUMN = 2;
const int TABLE_TYPE_COLUMN = 3;

LONG WINAPI MyUnhandledExceptionFilter( EXCEPTION_POINTERS* pExceptionInfo )
{
    MessageBoxA( nullptr, "Unhandled exception occurred!", "Crash Intercepted", MB_OK );

    // Optional: write a minidump
    HANDLE hFile = CreateFile( _T( "MemoryStressCrash.dmp" ), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr );
    if( hFile != INVALID_HANDLE_VALUE )
    {
        MINIDUMP_EXCEPTION_INFORMATION mdei;
        mdei.ThreadId = GetCurrentThreadId();
        mdei.ExceptionPointers = pExceptionInfo;
        mdei.ClientPointers = FALSE;

        MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &mdei, nullptr, nullptr );
        CloseHandle( hFile );
    }

    return EXCEPTION_EXECUTE_HANDLER;
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved )
{
    if( ul_reason_for_call == DLL_PROCESS_ATTACH )
    {
        SetUnhandledExceptionFilter( MyUnhandledExceptionFilter );
    }
    return TRUE;
}

extern "C" __declspec( dllexport )
int __cdecl RunQueryStress( const char* dsn, const char* user, const char* password, const char* database, const char* query )
{
    SQLHENV hEnv;
    SQLHDBC hDbc;
    SQLHSTMT hStmt;
    SQLRETURN ret;

    SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv );
    SQLSetEnvAttr( hEnv, SQL_ATTR_ODBC_VERSION, ( void* ) SQL_OV_ODBC3, 0 );
    SQLAllocHandle( SQL_HANDLE_DBC, hEnv, &hDbc );

    ret = SQLConnectA( hDbc,
        ( SQLCHAR* ) dsn, SQL_NTS,
        ( SQLCHAR* ) user, SQL_NTS,
        ( SQLCHAR* ) password, SQL_NTS );

    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        SQLFreeHandle( SQL_HANDLE_DBC, hDbc );
        SQLFreeHandle( SQL_HANDLE_ENV, hEnv );
        return AUTH_ERROR;
    }

    SQLAllocHandle( SQL_HANDLE_STMT, hDbc, &hStmt );

    // Switch to specified database
    std::string useDbCmd = "USE " + std::string( database );

    ret = SQLExecDirectA( hStmt, ( SQLCHAR* ) useDbCmd.c_str(), SQL_NTS );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        SQLFreeHandle( SQL_HANDLE_STMT, hStmt );
        SQLDisconnect( hDbc );
        SQLFreeHandle( SQL_HANDLE_DBC, hDbc );
        SQLFreeHandle( SQL_HANDLE_ENV, hEnv );
        return USE_DATABASE_ERROR;
    }

    // Execute the main query
    ret = SQLExecDirectA( hStmt, ( SQLCHAR* ) query, SQL_NTS );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        SQLFreeHandle( SQL_HANDLE_STMT, hStmt );
        SQLDisconnect( hDbc );
        SQLFreeHandle( SQL_HANDLE_DBC, hDbc );
        SQLFreeHandle( SQL_HANDLE_ENV, hEnv );
        return QUERY_ERROR;
    }

    while( SQLFetch( hStmt ) != SQL_NO_DATA )
    {
    }

    SQLFreeHandle( SQL_HANDLE_STMT, hStmt );
    SQLDisconnect( hDbc );
    SQLFreeHandle( SQL_HANDLE_DBC, hDbc );
    SQLFreeHandle( SQL_HANDLE_ENV, hEnv );

    return ( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO ) ? 0 : QUERY_ERROR;
}

extern "C" __declspec( dllexport )
int __cdecl RunDescribe( const char* dsn, const char* user, const char* password, const char* database, const char* schema )
{
    SQLHENV hEnv;
    SQLHDBC hDbc;
    SQLHSTMT hStmt;
    SQLRETURN ret;

    SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv );
    SQLSetEnvAttr( hEnv, SQL_ATTR_ODBC_VERSION, ( void* ) SQL_OV_ODBC3, 0 );
    SQLAllocHandle( SQL_HANDLE_DBC, hEnv, &hDbc );

    ret = SQLConnectA( hDbc,
        ( SQLCHAR* ) dsn, SQL_NTS,
        ( SQLCHAR* ) user, SQL_NTS,
        ( SQLCHAR* ) password, SQL_NTS );

    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        SQLFreeHandle( SQL_HANDLE_DBC, hDbc );
        SQLFreeHandle( SQL_HANDLE_ENV, hEnv );
        return AUTH_ERROR;
    }

    SQLAllocHandle( SQL_HANDLE_STMT, hDbc, &hStmt );

    // Switch to specified database
    std::string useDbCmd = "USE " + std::string( database );

    ret = SQLExecDirectA( hStmt, ( SQLCHAR* ) useDbCmd.c_str(), SQL_NTS );
    if( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO )
    {
        SQLFreeHandle( SQL_HANDLE_STMT, hStmt );
        SQLDisconnect( hDbc );
        SQLFreeHandle( SQL_HANDLE_DBC, hDbc );
        SQLFreeHandle( SQL_HANDLE_ENV, hEnv );
        return USE_DATABASE_ERROR;
    }

    std::set<CString> tableList;

    auto pst = new CODStatement( hDbc );

    try
    {
        // Describe the tables in the databases
        if( pst->DescribeTables( database, schema ) )
        {
            CString TableName;
            CString TableType;
            CString SchemaName;

            while( pst->Fetch() )
            {
                pst->GetResultset()->GetColumn( TABLE_NAME_COLUMN )->GetString( &TableName );
                pst->GetResultset()->GetColumn( TABLE_TYPE_COLUMN )->GetString( &TableType );
                pst->GetResultset()->GetColumn( SCHEMA_NAME_COLUMN )->GetString( &SchemaName );

                tableList.insert( TableName );
            }

            pst->Release();
        }

        
    }
    catch( ... )
    {
        delete pst;
        return EXCEPTION_DESCRIBING_TABLES;
    }
    
    delete pst;

    return tableList.size() > 0 ? SUCCESS_RESULT : NO_TABLES_ERROR;
}