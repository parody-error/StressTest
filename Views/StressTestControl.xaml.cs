using System.Text.RegularExpressions;
using System.Windows.Controls;
using System.Windows.Input;

namespace StressTest.Views
{
    public partial class StressTestControl : UserControl
    {
        public void NumberValidationTextBox( object sender, TextCompositionEventArgs e )
        {
            Regex regex = new Regex( "[^0-9]+" );
            e.Handled = regex.IsMatch( e.Text );
        }

        public StressTestControl()
        {
            InitializeComponent();
        }

        private void MemoryInMB_PreviewTextInput( object sender, TextCompositionEventArgs e )
        {

        }
    }
}
