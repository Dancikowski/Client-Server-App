using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Client.Views
{
    class BGWorker: BackgroundWorker
    {
        string mypath;
        ProgressBarWindow progressBar;
        public BGWorker(string path, ProgressBarWindow bar)
        {
            mypath = path;
            progressBar = bar;
        }

        public string Mypath { get => mypath; }
        public ProgressBarWindow ProgressBar { get => progressBar; set => progressBar = value; }
    }
}
