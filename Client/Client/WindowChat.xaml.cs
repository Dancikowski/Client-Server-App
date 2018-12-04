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
using System.Net.Sockets;
using System.Net;
using System.IO;
using System.ComponentModel;
using Client.Views;
using System.Threading;
using Microsoft.Win32;

namespace Client
{
    /// <summary>
    /// Interaction logic for WindowChat.xaml
    /// </summary>
    public partial class WindowChat : Window
    {
        
        public TcpClient Client;
        public NetworkStream Stream;
        byte[] buffer = new byte[1024];
                
        public WindowChat(TcpClient tcpClient, NetworkStream stream)
        {
            InitializeComponent();
            Client = tcpClient;
            Stream = stream;
            
        }
              
        public void SendCommand(string command)
        {
            try
            {
                byte[] commands = Encoding.ASCII.GetBytes(command);
                Stream.Write(commands, 0, commands.Length);
            }
            catch (Exception)
            {

                
            }
          
        }
        
        private void DisconnectButton_Click(object sender, RoutedEventArgs e)
        {

            SendCommand(":exit");
            Stream.Close();
            Client.Close();
            this.Close();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                SendCommand(":file");
                byte[] fileName = new byte[100];
                byte[] fileCount = new byte[64];
                Stream.Read(fileCount, 0, sizeof(Int32));
                var counter = BitConverter.ToInt32(fileCount, 0);
                Array.Clear(fileCount, 0, fileCount.Length);

                List<FileView> AllFileNames = new List<FileView>();


                for (int i = 0; i < counter; i++)
                {
                    Stream.Read(fileCount, 0, sizeof(Int32));
                    var SizeofFile = BitConverter.ToInt32(fileCount, 0);
                    Stream.Read(fileName, 0, SizeofFile);
                    var word = Encoding.UTF8.GetString(fileName);
                    word = word.Substring(0, word.IndexOf("\0"));

                    FileView file = new FileView(word, SizeofFile);
                    file.ShowOpenDir += DownloadFile;
                    MessageListBox.Items.Add(file);
                    AllFileNames.Add(file);


                    //AllFileNames.Add(word);
                    //MessageListBox.Items.Add(word);
                    Array.Clear(fileName, 0, fileName.Length);
                    Array.Clear(fileCount, 0, fileCount.Length);


                }
            }
            catch (Exception)
            {

                
            }
            
        }
        
        private void DownloadFile(string path)
        {
            ProgressBarWindow progressView = new ProgressBarWindow();
            progressView.Show();
            progressView.SendCancel += CancelSendingFile;
            progressView.FileNameTextBlock.Text = path.Substring(path.LastIndexOf("/") + 1);

            BGWorker worker = new BGWorker(path, progressView);
            worker.RunWorkerCompleted += worker_RunWorkerCompleted;
            worker.WorkerReportsProgress = true;
            worker.DoWork += worker_DoWork;
            worker.ProgressChanged += worker_ProgressChanged;
            worker.RunWorkerAsync();
        }

        private void CancelSendingFile()
        {
            Stream.Close();
            this.Close();
        }

        private void worker_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            
            var barView = sender as BGWorker;
            barView.ProgressBar.FileDownloadProgressBar.Value = e.ProgressPercentage;
            
        }

        private void worker_DoWork(object sender, DoWorkEventArgs e)
        {
            try
            {
                var worker = sender as BGWorker;
                SendCommand(":download");
                byte[] RecData = new byte[1024];
                byte[] buff = Encoding.ASCII.GetBytes(worker.Mypath);
                int RecBytes;
                Stream.Write(buff, 0, buff.Length);

                string fileName = worker.Mypath.Substring(worker.Mypath.LastIndexOf("/") + 1);


                byte[] fileSizeByte = new byte[64];

                Stream.Read(fileSizeByte, 0, sizeof(long));
                var fileSize = BitConverter.ToInt64(fileSizeByte, 0);

                Stream.Flush();

                
                long totalrecbytes = 0;
                FileStream Fs = new FileStream(Environment.GetFolderPath(Environment.SpecialFolder.Desktop)+@"\" + fileName, FileMode.OpenOrCreate, FileAccess.Write);
                
                int status = 0;
                int progress = 1;
                worker.ReportProgress(0);
                int part = (int)fileSize / 100;


                while (fileSize > totalrecbytes)
                {

                    RecBytes = Stream.Read(RecData, 0, RecData.Length);
                    Fs.Write(RecData, 0, RecBytes);
                    totalrecbytes += RecBytes;
                    //status = (totalrecbytes / fileSize) * 100;
                    //Thread.Sleep(1000);
                    if (totalrecbytes >= part * progress)
                    {
                        for (int i = 0; i < totalrecbytes / (part * progress); i++)
                        {
                            status++;
                            worker.ReportProgress(status);
                        }
                        progress++;
                    }
                }
                Fs.Close();
                
                worker.ReportProgress(100);
            }
            catch (Exception ex)
            {

                //MessageBox.Show(ex.Message);
            }
            
        }

        private void worker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            MessageBox.Show("Ukończono");
            var close = sender as BGWorker;
            close.ProgressBar.Close();
            
        }


    }
}
