using Prism.Commands;
using System;
using System.Threading.Tasks;
using System.Windows;

namespace StressTest.ViewModels
{
    internal class StressTestViewModel : ViewModelBase
    {
        public string SourceDSN { get; set; }

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

        private string _statusMessage = string.Empty;
        public string StatusMessage
        {
            get => _statusMessage;
            set
            {
                _statusMessage = value;
                OnPropertyChanged( nameof(StatusMessage) );
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

                Application.Current.Dispatcher.BeginInvoke( new Action( () =>
                {
                    StatusMessage = $"Allocating {memoryInMB} MB of memory";
                } ) );

                Task.Run( () =>
                {
                    NativeMethods.RunMemoryStress( memoryInMB, 10 );

                    // Clear StatusMessage after the operation completes
                    Application.Current.Dispatcher.Invoke( () =>
                    {
                        StatusMessage = string.Empty;
                    } );
                } );

            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error );
            }
            finally
            {
                Application.Current.Dispatcher.Invoke( () =>
                {
                    StatusMessage = string.Empty;
                } );
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
