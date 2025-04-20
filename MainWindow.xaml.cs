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
            }
            else
            {
                Close();
            }
        }
    }
}
