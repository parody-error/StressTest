using StressTest.ViewModels;
using StressTest.Views;
using System.Windows;

namespace StressTest
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            var mainVM = new MainViewModel();
            DataContext = mainVM;

            var connectionPrompt = new ConnectionDialog();
            var result = connectionPrompt.ShowDialog() ?? false;

            if ( result )
            {
                mainVM.StressTestViewModel.SourceDSN = connectionPrompt.SourceDSN;
                mainVM.StressTestViewModel.Username = connectionPrompt.Username;
                mainVM.StressTestViewModel.Password = connectionPrompt.Password;
                mainVM.StressTestViewModel.Database = connectionPrompt.Database;
            }
            else
            {
                Close();
            }
        }
    }
}
