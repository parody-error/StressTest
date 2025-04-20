using Prism.Commands;

namespace StressTest.ViewModels
{
    internal class ConnectionViewModel : ViewModelBase
    {
        public delegate void OnConnectAction( string sourceDSN );
        public event OnConnectAction OnConnectEvent;

        public delegate void OnCancelAction();
        public event OnCancelAction OnCancelEvent;

        public DelegateCommand ConnectCommand { get; }
        public DelegateCommand CancelCommand { get; }

        private string _sourceDSN = string.Empty;
        public string SourceDSN
        {
            get => _sourceDSN;
            set
            {
                _sourceDSN = value;
                UpdateCommandState();
            }
        }

        public ConnectionViewModel()
        {
            ConnectCommand = new DelegateCommand(
                ExecuteConnectCommand,
                () => { return !string.IsNullOrEmpty( _sourceDSN ); }
            );

            CancelCommand = new DelegateCommand(
                ExecuteCancelCommand,
                () => { return true; }
            );
        }

        private void ExecuteConnectCommand()
        {
            OnConnectEvent?.Invoke( _sourceDSN );
        }

        private void ExecuteCancelCommand()
        {
            OnCancelEvent?.Invoke();
        }

        private void UpdateCommandState()
        {
            ConnectCommand.RaiseCanExecuteChanged();
        }
    }
}
