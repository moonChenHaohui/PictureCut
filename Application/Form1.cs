using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

using System.ComponentModel;
using System.Data;
using System.Drawing;

using System.Text;

using System.Runtime.InteropServices;
using System.Drawing.Imaging;
using System.IO;

namespace WindowsFormsApplication1
{
    public partial class Form1 : Form
    {
       
        [DllImport("bmpTojpgNew.dll", CallingConvention = CallingConvention.Cdecl, SetLastError = true)]
        public static extern void output3(byte[] data, int width, int height, string out_);

        struct RESULT_{
            int width;
            int height;
            byte[] data;
        }

        private Form show = new Form();

        public Form1()
        {
            InitializeComponent();
        }




        public static byte[] RGB2bytes(Bitmap srcBitmap)
        {

            int wide = srcBitmap.Width;

            int height = srcBitmap.Height;

            Rectangle rect = new Rectangle(0, 0, wide, height);

            // 将Bitmap锁定到系统内存中, 获得BitmapData

            BitmapData srcBmData = srcBitmap.LockBits(rect,

                      ImageLockMode.ReadWrite, PixelFormat.Format24bppRgb);

            /*//创建Bitmap

             Bitmap dstBitmap = CreateGrayscaleImage(wide, height);//这个函数在后面有定义

            BitmapData dstBmData = dstBitmap.LockBits(rect,

                      ImageLockMode.ReadWrite, PixelFormat.Format8bppIndexed);*/

            // 位图中第一个像素数据的地址。它也可以看成是位图中的第一个扫描行

            System.IntPtr srcPtr = srcBmData.Scan0;

            //   System.IntPtr dstPtr = dstBmData.Scan0;

            // 将Bitmap对象的信息存放到byte数组中

            int src_bytes = srcBmData.Stride * height;

            byte[] srcValues = new byte[src_bytes];

            // int dst_bytes = dstBmData.Stride * height;

            //  byte[] dstValues = new byte[dst_bytes];

            //复制GRB信息到byte数组

            System.Runtime.InteropServices.Marshal.Copy(srcPtr, srcValues, 0, src_bytes);

           

            // 解锁位图

            srcBitmap.UnlockBits(srcBmData);

            //  dstBitmap.UnlockBits(dstBmData);

            // return dstBitmap;
            return srcValues;



        }

        private void Form1_Load(object sender, System.EventArgs e)
        {

        }

        private void button1_Click_1(object sender, System.EventArgs e)
        {
            
            string getStr = textBox1.Text;
            if(!File.Exists(getStr)){
                log(getStr + "-文件不存在");
                return;
            }
            string sor = getStr;
            string sstr = getStr + "[create].bmp";
            log(getStr + "-开始处理");
            Bitmap curBitmap = null;

            try
            {
                System.Drawing.Image temp = System.Drawing.Image.FromFile(sor);
                curBitmap = new System.Drawing.Bitmap(temp);
                temp.Dispose();
            }
            catch (Exception exp)
            {
                MessageBox.Show(exp.Message);
            }
            // PictureBox pb = pictureBox1;

            int iw = curBitmap.Width;
            int ih = curBitmap.Height;
            //DateTime dt = DateTime.Now;
            //long start = dt.Ticks;
            ///图片转换成字节流
            byte[] imgdata = RGB2bytes(curBitmap);

            output3(imgdata, iw, ih, sstr);
            log(getStr + "-图像处理完成.");
            
            if (checkBox1.Checked)
            {
                new Show(sor, sstr).Show();
            }
           
        }

        private void textBox1_TextChanged(object sender, System.EventArgs e)
        {

        }


       
        public void ListFiles(FileSystemInfo info)
        {
            if (!info.Exists) {
                MessageBox.Show("目录不存在;");
                return;
            }

            DirectoryInfo dir = info as DirectoryInfo;
            //不是目录
            if (dir == null) return;

            FileSystemInfo[] files = dir.GetFileSystemInfos();
            progressBar1.Maximum = files.Length;//设置最大长度值
            progressBar1.Value = 0;//设置当前值
            progressBar1.Step = 1;//设置没次增长多少
            for (int i = 0; i < files.Length; i++)
            {
                FileInfo file = files[i] as FileInfo;
                //是文件
                if (file != null)
                {
                 
                    if (file.FullName.Substring(file.FullName.LastIndexOf(".")) == ".bmp")
                    //此处为显示BMP格式
                    {
                        Bitmap curBitmap = null;

                        try
                        {
                            System.Drawing.Image temp = System.Drawing.Image.FromFile(file.FullName);
                            curBitmap = new System.Drawing.Bitmap(temp);
                            temp.Dispose();
                        }
                        catch (Exception exp)
                        {
                            MessageBox.Show(exp.Message);
                        }

                        int iw = curBitmap.Width;
                        int ih = curBitmap.Height;
                        //DateTime dt = DateTime.Now;
                        //long start = dt.Ticks;
                        ///图片转换成字节流
                        byte[] imgdata = RGB2bytes(curBitmap);
                        log("处理图像[" + file.FullName + "] 开始.");
                        output3(imgdata, iw, ih, file.FullName + "[create].bmp");
                        log("处理图像[" + file.FullName + "] 结束.");
                        imgdata = null;
                        GC.Collect();
                    }
                }
                //对于子目录
                else
                {
                    //ListFiles(files[i]);
                }
                progressBar1.Value += progressBar1.Step;//让进度条增加一次
            }

        }

        private void button2_Click_1(object sender, System.EventArgs e)
        {
            string sor = textBox2.Text;
            log("批处理开始");
            ListFiles(new DirectoryInfo(sor));
            log("批处理完毕");
            System.Diagnostics.Process.Start(sor);
        }


        private void textBox2_TextChanged(object sender, System.EventArgs e)
        {

        }



        private void folderBrowserDialog1_HelpRequest(object sender, System.EventArgs e)
        {

        }

        private void button3_Click(object sender, System.EventArgs e)
        {
            OpenFileDialog fileDialog = new OpenFileDialog();

            fileDialog.InitialDirectory = "C://";

            fileDialog.Filter = "bmp (*.bmp)|*.bmp";

            fileDialog.FilterIndex = 1;

            fileDialog.RestoreDirectory = true;

            if (fileDialog.ShowDialog() == DialogResult.OK)
            {

                this.textBox1.Text = fileDialog.FileName;

            }
        }

        private void button4_Click(object sender, System.EventArgs e)
        {
            string path = "";
            FolderBrowserDialog folderBrowserDialog = new FolderBrowserDialog();
            folderBrowserDialog.ShowNewFolderButton = false;
            if (folderBrowserDialog.ShowDialog() == DialogResult.OK)
            {
                path = folderBrowserDialog.SelectedPath;
            }
            else
            {
            }
            this.textBox2.Text = path;

        }
        int count = 1;
        private void log(String str) {
            this.textBox3.AppendText(count++ + ":" + str + '\n');
        }

        private void button5_Click(object sender, EventArgs e)
        {
            this.textBox3.Text = "";
        }

        private void button6_Click(object sender, EventArgs e)
        {

        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void progressBar1_Click(object sender, EventArgs e)
        {

        }
    }
}
