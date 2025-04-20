using Prism.Commands;

namespace StressTest.ViewModels
{
    internal class ConnectionViewModel : ViewModelBase
    {
        public delegate void OnSelectAction( string sourceDSN, string username, string password );
        public event OnSelectAction OnSelectEvent;

        public delegate void OnCancelAction();
        public event OnCancelAction OnCancelEvent;

        public DelegateCommand SelectCommand { get; }
        public DelegateCommand CancelCommand { get; }

        private string _sourceDSN = "StressTestDSN_ss"; //#sb:  string.Empty;
        public string SourceDSN
        {
            get => _sourceDSN;
            set
            {
                _sourceDSN = value;
                UpdateCommandState();
            }
        }

        private string _username = "ss_auth"; //#SB: string.Empty;
        public string Username
        {
            get => _username;
            set
            {
                _username = value;
                UpdateCommandState();
            }
        }

        private string _password = "ss_pass"; //#SB: string.Empty;
        public string Password
        {
            get => _password;
            set
            {
                _password = value;
                UpdateCommandState();
            }
        }

        public ConnectionViewModel()
        {
            SelectCommand = new DelegateCommand(
                ExecuteSelectCommand,
                () => {
                    return !string.IsNullOrEmpty( _sourceDSN ) &&
                           !string.IsNullOrEmpty( _username ) &&
                           !string.IsNullOrEmpty( _password );
                }
            );

            CancelCommand = new DelegateCommand(
                ExecuteCancelCommand,
                () => { return true; }
            );
        }

        private void ExecuteSelectCommand()
        {
            OnSelectEvent?.Invoke( _sourceDSN, _username, _password );
        }

        private void ExecuteCancelCommand()
        {
            OnCancelEvent?.Invoke();
        }

        private void UpdateCommandState()
        {
            SelectCommand.RaiseCanExecuteChanged();
        }
    }
}
