using StressTest.ViewModels;
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

            //#SB: add connection dialog behaviour.
        }
    }
}
