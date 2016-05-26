using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace WindowsFormsApplication1
{
    public partial class Show : Form
    {
        public Show() { }
        public Show(string a,string b)
        {
            InitializeComponent();
            init(a,b);
        }

        private void init(string a,string b) {
            System.Drawing.Image img = System.Drawing.Image.FromFile(@"" + a);
            System.Drawing.Image bmp = new System.Drawing.Bitmap(img);
            pictureBox1.Image = bmp;
            img.Dispose();

            //反转图片
            pictureBox1.Image.RotateFlip(RotateFlipType.Rotate180FlipNone);

            img = System.Drawing.Image.FromFile("canny_.jpg");
            bmp = new System.Drawing.Bitmap(img);
            pictureBox2.Image = bmp;
            img.Dispose();

            img = System.Drawing.Image.FromFile("增强_canny_中值滤波后结果1.bmp");
            bmp = new System.Drawing.Bitmap(img);
            pictureBox3.Image = bmp;
            img.Dispose();


            img = System.Drawing.Image.FromFile(@"" + b);
            bmp = new System.Drawing.Bitmap(img);
            pictureBox4.Image = bmp;
            img.Dispose();

       
        }

        private void Show_Load(object sender, EventArgs e)
        {

        }

        private void pictureBox1_Click(object sender, EventArgs e)
        {

        }

        private void pictureBox3_Click(object sender, EventArgs e)
        {

        }

        private void groupBox2_Enter(object sender, EventArgs e)
        {

        }

        private void pictureBox4_Click(object sender, EventArgs e)
        {

        }

        private void groupBox3_Enter(object sender, EventArgs e)
        {

        }

        private void groupBox1_Enter(object sender, EventArgs e)
        {

        }
        public void Dispose() {
            this.pictureBox1.Image.Dispose();
            this.pictureBox1.Image = null;
            this.pictureBox2.Image.Dispose();
            this.pictureBox2.Image = null;
            this.pictureBox3.Image.Dispose();
            this.pictureBox3.Image = null;
            this.pictureBox4.Image.Dispose();
            this.pictureBox4.Image = null;
            Dispose(true);
            
        }
    }
}
