using Prism.Commands;

namespace StressTest.ViewModels
{
    internal class ConnectionViewModel : ViewModelBase
    {
        public delegate void OnSelectAction( string sourceDSN );
        public event OnSelectAction OnSelectEvent;

        public delegate void OnCancelAction();
        public event OnCancelAction OnCancelEvent;

        public DelegateCommand SelectCommand { get; }
        public DelegateCommand CancelCommand { get; }

        private string _sourceDSN = "StressTestDSN"; //#sb:  string.Empty;
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
            OnSelectEvent?.Invoke( _sourceDSN );
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
