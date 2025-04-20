#include "pch.h"
#include <windows.h>
#include <vector>
#include <cstring>
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

extern "C" __declspec( dllexport ) int RunMemoryStress( int megabytesToAllocate )
{
    try {
        size_t bytes = static_cast<size_t>( megabytesToAllocate ) * 1024 * 1024;
        std::vector<char*> allocations;
        size_t chunkSize = 10 * 1024 * 1024;
        size_t allocated = 0;

        while ( allocated < bytes ) {
            char* chunk = (char*)malloc( chunkSize );
            if ( !chunk ) break;
            memset( chunk, 0xAA, chunkSize ); // Touch the memory
            allocations.push_back( chunk );
            allocated += chunkSize;
        }

        // Simulate some delay
        Sleep( 2000 );

        for ( char* chunk : allocations ) {
            free( chunk );
        }

        return static_cast<int>( allocated / ( 1024 * 1024 ) ); // Return actual MB allocated
    }
    catch ( ... ) {
        return -1;
    }
}
