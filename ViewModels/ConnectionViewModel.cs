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

        public string Username { get; set; } = "ss_auth"; //#SB: string.Empty;
        public string Password { get; set; } = "ss_pass"; //#SB: string.Empty;

        public ConnectionViewModel()
        {
            SelectCommand = new DelegateCommand(
                ExecuteSelectCommand,
                () => { return !string.IsNullOrEmpty( _sourceDSN ); }
            );

            CancelCommand = new DelegateCommand(
                ExecuteCancelCommand,
                () => { return true; }
            );
        }

        private void ExecuteSelectCommand()
        {
            OnSelectEvent?.Invoke( _sourceDSN, Username, Password );
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
