using Prism.Commands;
using System;

namespace StressTest.ViewModels
{
    internal class StressTestViewModel : ViewModelBase
    {
        public DelegateCommand AllocateCommand { get; }
        private bool CanExecuteAllocateCommand => true; //#SB: make sure we have a non-negative memory specified.

        public StressTestViewModel()
        {
            AllocateCommand = new DelegateCommand(
                ExecuteAllocateCommand,
                () => CanExecuteAllocateCommand
            );
        }

        private void ExecuteAllocateCommand()
        {
            if ( !CanExecuteAllocateCommand )
                return;

            Console.WriteLine( "Allocating" );
        }
    }
}
