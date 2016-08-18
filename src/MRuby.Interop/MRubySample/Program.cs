using System;
using System.Collections.Generic;
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
            using (var mruby = new MRuby())
            {
                Console.WriteLine(mruby.LoadStringW(Console.ReadLine()));
            }

            Console.ReadLine();
        }
    }
}
