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


namespace Client
{
    /// <summary>
    /// Interaction logic for FileView.xaml
    /// </summary>
    public partial class FileView : UserControl
    {
        string path = "/home/damian/Downloads/";
        string fullpath;

        public string Fullpath { get => fullpath; set => fullpath = value; }
        
        //FileView fileview = new FileView();
        public FileView(string name, int size)
        {
            InitializeComponent();
            FileNameTextBlock.Text = name;
            // FileSizeNameBlock.Text = size.ToString();
            fullpath = path + name;
            

        }

        public delegate void showOpenEventHandler(string path);
        public event showOpenEventHandler ShowOpenDir;


        protected virtual void OnShowOpenedDir()
        {
            if (ShowOpenDir != null)
            {
                ShowOpenDir.Invoke(Fullpath);
            }
        }


        private void UserControl_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {

            OnShowOpenedDir();


        }
    }
}
