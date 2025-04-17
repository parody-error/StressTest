namespace StressTest.Models
{
    internal class StressTest
    {
        private const int DefaultMemory = 16 * 1024 * 1024; // 16 MB

        public string Query { get; set; } = string.Empty;
        public int Memory { get; set; } = DefaultMemory;
    }
}
