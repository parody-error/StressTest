#include "pch.h"
#include <windows.h>
#include <vector>
#include <cstring>

//#SB: if this works, just use new/delete probably
extern "C" __declspec( dllexport ) int RunMemoryStress( int megabytesToAllocate )
{
    try {
        size_t bytes = static_cast<size_t>( megabytesToAllocate ) * 1024 * 1024;
        std::vector<char*> allocations;
        size_t chunkSize = 10 * 1024 * 1024; // 10MB at a time
        size_t allocated = 0;

        while ( allocated < bytes ) {
            char* chunk = (char*)malloc( chunkSize );
            if ( !chunk ) break;
            memset( chunk, 0xAA, chunkSize ); // Touch the memory
            allocations.push_back( chunk );
            allocated += chunkSize;
        }

        // Simulate some delay
        Sleep( 2000 ); //#SB: reduce this, just testing

        for ( char* chunk : allocations ) {
            free( chunk );
        }

        return static_cast<int>( allocated / ( 1024 * 1024 ) ); // Return actual MB allocated
    }
    catch ( ... ) {
        return -1;
    }
}
