using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace MRubySample
{
    public class ShowWindow
    {
        public static void Show(String message)
        {
            var form = new Form();
            form.Size = new Size(200, 200);

            form.PerformLayout();

            var label = new Label();
            label.Text = message;
            label.Dock = DockStyle.Fill;
            label.TextAlign = ContentAlignment.MiddleCenter;
            form.Controls.Add(label);

            form.ResumeLayout();

            form.Show();
        }
    }
}
