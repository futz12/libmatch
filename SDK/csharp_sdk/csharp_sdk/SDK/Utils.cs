using System;
using System.Collections.Generic;
using System.Drawing.Imaging;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace LibMatch
{
    public static class Utils
    {
        public static IntPtr BytesToPtr(byte[] array)
        {
            return Marshal.UnsafeAddrOfPinnedArrayElement(array, 0);
        }
    }
}
