using StressTest.ViewModels;
using System.Windows;

namespace StressTest.Views
{
    public partial class ConnectionDialog : Window
    {
        public string SourceDSN { get; private set; } = string.Empty;

        public ConnectionDialog()
        {
            InitializeComponent();

            var connectionVM = new ConnectionViewModel();
            DataContext = connectionVM;

            connectionVM.OnConnectEvent += OnConnect;
            connectionVM.OnCancelEvent += OnCancel;
        }

        public void Dispose()
        {
            var connectionVM = DataContext as ConnectionViewModel;
            if ( connectionVM != null )
            {
                connectionVM.OnConnectEvent -= OnConnect;
                connectionVM.OnCancelEvent -= OnCancel;
            }
        }

        private void OnConnect( string sourceDSN )
        {
            SourceDSN = sourceDSN;
            DialogResult = !string.IsNullOrEmpty( sourceDSN );
        }

        private void OnCancel()
        {
            DialogResult = false;
        }
    }
}
