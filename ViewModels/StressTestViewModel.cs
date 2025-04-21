using Prism.Commands;
using System;
using System.Threading.Tasks;
using System.Windows;

namespace StressTest.ViewModels
{
    internal class StressTestViewModel : ViewModelBase
    {
        private const int UNKNOWN_RESULT = -9;
        private const int SUCCESS_RESULT = 0;
        private const int AUTH_ERROR = -1;
        private const int USE_DATABASE_ERROR = -2;
        private const int QUERY_ERROR = -3;
        private const int ODBC_ERROR = -4;

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

        private string _database = string.Empty;
        public string Database
        {
            get => _database;
            set
            {
                _database = value;
                QueryCommand.RaiseCanExecuteChanged();
            }
        }

        public string Username { get; set; } = string.Empty;
        public string Password { get; set; } = string.Empty;

        private string _query =
@"select
  PROPNUM,
  LEASE,
  STATE,
  COUNTY,
  FIELD,
  RESERVOIR,
  PROPNUM,
  API_14,
  LEASE,
  OPERATOR,
  OPERATED_FLAG,
  ASSET,
  ENERTIA_ID,
  BUDGET_YEAR,
  RESERVOIR_CATEGORY,
  ARIES_SOURCE
from
  ANALYTICS.DIM_AC_PROPERTY
where
  PROPNUM is not NULL and 
  ( RESERVOIR in ('BOSSIER', 'HAYNESVILLE SHALE', 'MID BOSSIER', 'UPR BOSSIER') ) and 
  ( OPERATED_FLAG in ('Y') ) and 
  ( BUDGET_YEAR in ('CURRENT') ) and 
  ( RESERVOIR_CATEGORY in ('1PDP', '2PDNP', '3PUD', '4PROB', '4PUDX', '5POSS', '6CON') )
";

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
        private bool CanExecuteQueryCommand => !string.IsNullOrEmpty( Query ) && !string.IsNullOrEmpty( SourceDSN ) && !string.IsNullOrEmpty( Database );

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

            int memoryInMB = int.Parse( _memoryInMB );

            try
            {
                await Application.Current.Dispatcher.BeginInvoke( new Action( () => { StatusMessage = $"Allocating {memoryInMB} MB of memory"; } ) );
                await Task.Run( () => { NativeMethods.RunMemoryStress( memoryInMB ); } );
            }
            catch ( Exception ex )
            {
                MessageBox.Show( $"Error: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error );
            }
            finally
            {
                Application.Current.Dispatcher.Invoke( () => { StatusMessage = $"Deallocated {memoryInMB} MB of memory"; } );
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
                await Task.Run( () => { result = NativeMethods.RunQueryStress( SourceDSN, Username, Password, Database, Query ); } );
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
                            StatusMessage = "Executed query successfully";
                            break;
                        case AUTH_ERROR:
                            StatusMessage = "Authentication error";
                            break;
                        case USE_DATABASE_ERROR:
                            StatusMessage = "Error using database";
                            break;
                        case QUERY_ERROR:
                            StatusMessage = "Error executing query";
                            break;
                        case ODBC_ERROR:
                            StatusMessage = "ODBC error";
                            break;
                        case UNKNOWN_RESULT:
                        default:
                            StatusMessage = "Unknown error encountered";
                            break;
                    }
                } );
            }
        }
    }
}
