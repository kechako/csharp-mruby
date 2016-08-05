using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using MRuby.Interop;

namespace TestApp
{
    class Program
    {
        static void Main(string[] args)
        {
            using (var mri = new MRubyInterpreter()) {
                mri.LoadStringW("class Test def add(x, y) x + y end end");

                var Test = mri.GetClass("Test");
                var test = Test.New();

                mri.FullGC();

                var value = test.Send("add", 10, 20);

                Console.WriteLine("{0}", value);
            }

            Console.ReadLine();
        }
    }
}
