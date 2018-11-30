using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Sockets;
using System.Net;
using System.IO;
 
namespace ConsoleApplication1
{
    class Program
    {
        static void Main(string[] args)
        {
            {
                TcpClient externalClient = new TcpClient();
                byte[] RecData = new byte[1024];
                byte[] fileSize;;
                int RecBytes;

                try
                {

                    externalClient.Connect("127.0.0.1", 4440);
                    string filepath = "/home/damian/Downloads/bilety_902956.pdf";
                    string fileName = filepath.Substring(filepath.LastIndexOf("/") + 1);
                    Console.WriteLine(fileName);

                    NetworkStream netstream = null;
                    netstream = externalClient.GetStream();

                    byte[] myWriteBuffer = Encoding.ASCII.GetBytes(filepath);
                    netstream.Write(myWriteBuffer, 0, myWriteBuffer.Length);

                    Console.WriteLine("Message sent!");

                   fileSize=  new byte[externalClient.ReceiveBufferSize];

                    String responseData = String.Empty;

                    int bytes = netstream.Read(fileSize, 0, fileSize.Length);
                    responseData = System.Text.Encoding.ASCII.GetString(fileSize, 0, bytes); //
                    Console.WriteLine(responseData);
                    netstream.Flush();


                    int totalrecbytes = 0;
                    FileStream Fs = new FileStream(fileName, FileMode.OpenOrCreate, FileAccess.Write);

                    while (Int32.Parse(responseData) > totalrecbytes)
                    {
                        RecBytes = netstream.Read(RecData, 0, RecData.Length);
                        Fs.Write(RecData, 0, RecBytes);
                        totalrecbytes += RecBytes;
                        Console.WriteLine(totalrecbytes);
                    }
                    Fs.Close();
                }

                catch (Exception ex)
                {
                    Console.WriteLine(ex);
                }
            }
        }
    }
}

