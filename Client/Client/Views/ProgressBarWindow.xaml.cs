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
using System.Windows.Shapes;

namespace Client.Views
{
    /// <summary>
    /// Interaction logic for ProgressBarWindow.xaml
    /// </summary>
    public partial class ProgressBarWindow : Window
    {
        public delegate void CancelRecivedFileEventHandler();
        public event CancelRecivedFileEventHandler SendCancel;

        protected virtual void OnSendCancel()
        {

            if(SendCancel != null)
            {
                SendCancel.Invoke();
            }

        }

        public ProgressBarWindow()
        {
            InitializeComponent();
        }

        private void CancelButton_Click(object sender, RoutedEventArgs e)
        {
            OnSendCancel();
        }
    }
}
