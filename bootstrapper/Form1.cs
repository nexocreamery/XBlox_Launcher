using System;
using System.Diagnostics;
using System.IO;
using System.Windows.Forms;

namespace bootstrapper
{
    public partial class Form1 : Form
    {
        public Form1(string[] args)
        {
            InitializeComponent();

            if (args.Length == 0)
                return;

            string cookiePath = Path.Combine(
                Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData),
                "XBlox",
                "LocalStorage",
                "XBloxCookies.dat"
            );

            if (!File.Exists(cookiePath))
            {
                MessageBox.Show("Cookie file not found.");
                return;
            }

            string cookie = File.ReadAllText(cookiePath);

            Uri uri = new Uri(args[0]);

            if (uri.Host == "join")
            {
                string query = uri.Query; // ?id=123
                string id = query.Replace("?id=", "");

                Process.Start(new ProcessStartInfo
                {
                    FileName = "CLI.exe",
                    Arguments = $"Cookie {cookie} {id}",
                    UseShellExecute = false
                });
            }
        }

        private void close_Click(object sender, EventArgs e)
        {
            Close();
        }
    }
}