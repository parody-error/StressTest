#include "pch.h"
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <string>
#include <DbgHelp.h>
#include <tchar.h>

#pragma comment(lib, "Dbghelp.lib")

LONG WINAPI MyUnhandledExceptionFilter( EXCEPTION_POINTERS* pExceptionInfo )
{
    MessageBoxA( nullptr, "Unhandled exception occurred!", "Crash Intercepted", MB_OK );

    // Optional: write a minidump
    HANDLE hFile = CreateFile( _T( "MemoryStressCrash.dmp" ), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr );
    if ( hFile != INVALID_HANDLE_VALUE )
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
    if ( ul_reason_for_call == DLL_PROCESS_ATTACH ) {
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
    SQLSetEnvAttr( hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0 );
    SQLAllocHandle( SQL_HANDLE_DBC, hEnv, &hDbc );

    ret = SQLConnectA( hDbc,
        (SQLCHAR*)dsn, SQL_NTS,
        (SQLCHAR*)user, SQL_NTS,
        (SQLCHAR*)password, SQL_NTS );

    if ( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO ) {
        SQLFreeHandle( SQL_HANDLE_DBC, hDbc );
        SQLFreeHandle( SQL_HANDLE_ENV, hEnv );
        return -1;
    }

    SQLAllocHandle( SQL_HANDLE_STMT, hDbc, &hStmt );

    // Switch to specified database
    std::string useDbCmd = "USE " + std::string( database );
    ret = SQLExecDirectA( hStmt, (SQLCHAR*)useDbCmd.c_str(), SQL_NTS );
    if ( ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO ) {
        SQLFreeHandle( SQL_HANDLE_STMT, hStmt );
        SQLDisconnect( hDbc );
        SQLFreeHandle( SQL_HANDLE_DBC, hDbc );
        SQLFreeHandle( SQL_HANDLE_ENV, hEnv );
        return -2;
    }

    // Execute the main query
    ret = SQLExecDirectA( hStmt, (SQLCHAR*)query, SQL_NTS );

    SQLFreeHandle( SQL_HANDLE_STMT, hStmt );
    SQLDisconnect( hDbc );
    SQLFreeHandle( SQL_HANDLE_DBC, hDbc );
    SQLFreeHandle( SQL_HANDLE_ENV, hEnv );

    return ( ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO ) ? 0 : -3;
}
