using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using AudioLib.WpfUi;

namespace Shapeshifter.Ui
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private MainViewModel vm;

        public MainWindow()
        {
            InitializeComponent();
            vm = new MainViewModel();
            DataContext = vm;

            var knobs = ChildFinder.GetChildrenOfType<Knob2>(KnobGrid).ToArray();

            foreach (var k in knobs)
            {
                k.MouseEnter += MouseOverKnob;
                k.MouseLeave += MouseLeaveKnob;
            }
        }

        private void MouseLeaveKnob(object sender, MouseEventArgs e)
        {
            vm.FocusedKnob = null;
        }

        private void MouseOverKnob(object sender, MouseEventArgs e)
        {
            var col = ((Knob2)sender).GetValue(Grid.ColumnProperty);
            vm.FocusedKnob = (int)col;
        }

        private void ExitHandler(object sender, MouseButtonEventArgs e)
        {
            Close();
        }
    }
}
