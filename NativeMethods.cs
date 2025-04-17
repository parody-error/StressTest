using System.Runtime.InteropServices;

public static class NativeMethods
{
    [DllImport( "MemoryStressTest.dll", CallingConvention = CallingConvention.Cdecl )]
    public static extern void RunMemoryStress( int megabytesToAllocate, int holdSeconds );
}