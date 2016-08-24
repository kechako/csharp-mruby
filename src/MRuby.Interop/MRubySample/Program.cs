using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using MRubyCSharp.Interop;

namespace MRubySample
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length == 0)
            {
                return;
            }

            var code = "";
            using (var reader = new StreamReader(args[0]))
            {
                code = reader.ReadToEnd();
            }

            using (var mruby = new MRuby())
            {
                mruby.LoadStringW(code);
            }

            Console.ReadLine();
        }
    }
}
