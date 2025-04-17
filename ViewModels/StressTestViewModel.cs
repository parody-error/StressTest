using Prism.Commands;
using System;

namespace StressTest.ViewModels
{
    internal class StressTestViewModel : ViewModelBase
    {
        private string _query = "SELECT * FROM WELL";
        public string Query
        {
            get => _query;
            set
            {
                _query = value;
                QueryCommand.RaiseCanExecuteChanged();
            }
        }

        public DelegateCommand AllocateCommand { get; }
        private bool CanExecuteAllocateCommand => true; //#SB: make sure we have a non-negative memory specified.

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

            Console.WriteLine( "Allocating" );
        }

        private void ExecuteQueryCommand()
        {
            if ( !CanExecuteQueryCommand )
                return;

            Console.WriteLine( "Querying" );
        }
    }
}
