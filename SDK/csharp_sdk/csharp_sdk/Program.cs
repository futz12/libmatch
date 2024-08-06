using LibMatch;
using System.Runtime.InteropServices;

namespace csharp_sdk
{

    internal class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Version: " + NativeApi.Version);

            byte[] Template = File.ReadAllBytes("eye.png");
            byte[] Source = File.ReadAllBytes("main.png");
            TemplateMatcher matcher = new TemplateMatcher(Template, MatchMode.COLOR_BGR);
            List<ObjectEx> result = matcher.Compute(Source);
            Console.WriteLine("Result: " + result.Count);
            for (int i = 0; i < result.Count; i++)
            {
                Console.WriteLine("-----------------------------------------------------------");
                Console.WriteLine("prob: " + result[i].prob.ToString());
                Console.WriteLine("rect:");
                Console.WriteLine("    x: " + result[i].rect.x);
                Console.WriteLine("    y: " + result[i].rect.y);
                Console.WriteLine("    width: " + result[i].rect.width);
                Console.WriteLine("    height :" + result[i].rect.height);
                Console.WriteLine("-----------------------------------------------------------");
            }
            Console.ReadLine();
        }
    }
}
