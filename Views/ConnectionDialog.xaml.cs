using StressTest.ViewModels;
using System.Windows;

namespace StressTest.Views
{
    public partial class ConnectionDialog : Window
    {
        public string SourceDSN { get; private set; } = string.Empty;
        public string Username { get; private set; } = string.Empty;
        public string Password { get; private set; } = string.Empty;
        public string Database { get; private set; } = string.Empty;

        public ConnectionDialog()
        {
            InitializeComponent();

            var connectionVM = new ConnectionViewModel();
            DataContext = connectionVM;

            connectionVM.OnSelectEvent += OnSelect;
            connectionVM.OnCancelEvent += OnCancel;
        }

        public void Dispose()
        {
            var connectionVM = DataContext as ConnectionViewModel;
            if ( connectionVM != null )
            {
                connectionVM.OnSelectEvent -= OnSelect;
                connectionVM.OnCancelEvent -= OnCancel;
            }
        }

        private void OnSelect( string sourceDSN, string username, string password, string database )
        {
            SourceDSN = sourceDSN;
            Username = username;
            Password = password;
            Database = database;

            DialogResult = !string.IsNullOrEmpty( SourceDSN ) && !string.IsNullOrEmpty( Database );
        }

        private void OnCancel()
        {
            DialogResult = false;
        }
    }
}
