using Prism.Commands;
using System;
using System.Windows;

namespace StressTest.ViewModels
{
    internal class StressTestViewModel : ViewModelBase
    {
        private string _query = "SELECT * FROM WELL"; //#SB: use a different query
        public string Query
        {
            get => _query;
            set
            {
                _query = value;
                QueryCommand.RaiseCanExecuteChanged();
            }
        }

        private string _memoryInMB = "500";
        public string MemoryInMB
        {
            get => _memoryInMB;
            set
            {
                _memoryInMB = value;
                AllocateCommand.RaiseCanExecuteChanged();
            }
        }

        public DelegateCommand AllocateCommand { get; }
        private bool CanExecuteAllocateCommand => !string.IsNullOrEmpty( _memoryInMB );

        public DelegateCommand QueryCommand { get; }
        private bool CanExecuteQueryCommand => !string.IsNullOrEmpty( Query );

        public StressTestViewModel()
        {
            AllocateCommand = new DelegateCommand(
                ExecuteAllocateCommand,
                () => CanExecuteAllocateCommand
            );

            QueryCommand = new DelegateCommand(
                ExecuteQueryCommand,
                () => CanExecuteQueryCommand
            );
        }

        private void ExecuteAllocateCommand()
        {
            if ( !CanExecuteAllocateCommand )
                return;

            try
            {
                int memoryInMB = int.Parse( _memoryInMB );
                NativeMethods.RunMemoryStress( memoryInMB, 10 );
                //#SB: maybe add something to the UI to show that the memory is being allocated.
                MessageBox.Show( "Done" );
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error );
            }
        }

        private void ExecuteQueryCommand()
        {
            if ( !CanExecuteQueryCommand )
                return;

            Console.WriteLine( "Querying" );
        }
    }
}
