using Prism.Commands;
using System;
using System.Threading.Tasks;
using System.Windows;

namespace StressTest.ViewModels
{
    internal class StressTestViewModel : ViewModelBase
    {
        private const int UNKNOWN_RESULT = -3;
        private const int SUCCESS_RESULT = 0;
        private const int AUTH_ERROR = -1;
        private const int QUERY_ERROR = -2;

        private string _sourceDSN = string.Empty;
        public string SourceDSN
        {
            get => _sourceDSN;
            set
            {
                _sourceDSN = value;
                QueryCommand.RaiseCanExecuteChanged();
            }
        }

        public string Username { get; set; } = string.Empty;
        public string Password { get; set; } = string.Empty;

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
                OnPropertyChanged( nameof( StatusMessage ) );
            }
        }

        public DelegateCommand AllocateCommand { get; }
        private bool CanExecuteAllocateCommand => !string.IsNullOrEmpty( _memoryInMB );

        public DelegateCommand QueryCommand { get; }
        private bool CanExecuteQueryCommand => !string.IsNullOrEmpty( Query ) && !string.IsNullOrEmpty( SourceDSN );

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

        private async void ExecuteAllocateCommand()
        {
            if ( !CanExecuteAllocateCommand )
                return;

            try
            {
                int memoryInMB = int.Parse( _memoryInMB );

                await Application.Current.Dispatcher.BeginInvoke( new Action( () => { StatusMessage = $"Allocating {memoryInMB} MB of memory"; } ) );
                await Task.Run( () => { NativeMethods.RunMemoryStress( memoryInMB ); } );

            }
            catch ( Exception ex )
            {
                MessageBox.Show( $"Error: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error );
            }
            finally
            {
                Application.Current.Dispatcher.Invoke( () => { StatusMessage = string.Empty; } );
            }
        }

        private async void ExecuteQueryCommand()
        {
            if ( !CanExecuteQueryCommand )
                return;

            int result = UNKNOWN_RESULT;

            try
            {
                await Application.Current.Dispatcher.BeginInvoke( new Action( () => { StatusMessage = "Executing query..."; } ) );
                await Task.Run( () => { result = NativeMethods.RunQueryStress( SourceDSN, Username, Password, Query ); } );
            }
            catch ( Exception ex )
            {
                MessageBox.Show( $"Error: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error );
            }
            finally
            {
                Application.Current.Dispatcher.Invoke( () =>
                {
                    switch ( result )
                    {
                        case SUCCESS_RESULT:
                            StatusMessage = "Success";
                            break;
                        case AUTH_ERROR:
                            StatusMessage = "Authentication error";
                            break;
                        case QUERY_ERROR:
                            StatusMessage = "Query error";
                            break;
                        case UNKNOWN_RESULT:
                            StatusMessage = "Unknown error";
                            break;
                        default:
                            StatusMessage = "Unknown error";
                            break;
                    }
                } );
            }
        }
    }
}
