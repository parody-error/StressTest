using System.Runtime.InteropServices;

public static class NativeMethods
{
    [DllImport("MemoryStressTest.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern void RunMemoryStress(int megabytesToAllocate);

    [DllImport("QueryStressTest.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int RunQueryStress(
        [MarshalAs(UnmanagedType.LPStr)] string dsn,
        [MarshalAs(UnmanagedType.LPStr)] string user,
        [MarshalAs(UnmanagedType.LPStr)] string password,
        [MarshalAs(UnmanagedType.LPStr)] string database,
        [MarshalAs(UnmanagedType.LPStr)] string query
    );

    [DllImport("QueryStressTest.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern int RunDescribe(
        [MarshalAs(UnmanagedType.LPStr)] string dsn,
        [MarshalAs(UnmanagedType.LPStr)] string user,
        [MarshalAs(UnmanagedType.LPStr)] string password,
        [MarshalAs(UnmanagedType.LPStr)] string database,
        [MarshalAs(UnmanagedType.LPStr)] string schema
    );

    [DllImport( "QueryStressTest.dll", CallingConvention = CallingConvention.Cdecl )]
    public static extern int RunThreadedDescribe(
        [MarshalAs( UnmanagedType.LPStr )] string dsn,
        [MarshalAs( UnmanagedType.LPStr )] string user,
        [MarshalAs( UnmanagedType.LPStr )] string password,
        [MarshalAs( UnmanagedType.LPStr )] string database,
        [MarshalAs( UnmanagedType.LPStr )] string schema
    );
}