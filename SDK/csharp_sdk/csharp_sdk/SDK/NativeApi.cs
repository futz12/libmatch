using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace LibMatch
{

    public struct Rect
    {
        public int x;
        public int y;
        public int width;
        public int height;
    }

    public struct ObjectEx
    {
        public Rect rect;
        public float prob;
    }

    public enum MatchMode
    {
        COLOR_GRAY,
        COLOR_BGRA,
        COLOR_BGR,
        COLOR_BGRA_COLOR,
        COLOR_BGR_MASK,
        COLOR_GRAY_MASK,
    }


    public static class NativeApi
    {
        /* 获取库文件版本号 */
        [DllImport("libmatch.dll")]
        public static extern IntPtr libmatch_version();

        /* 创建匹配模板 */
        [DllImport("libmatch.dll")]
        public static extern IntPtr create_template_matcher(IntPtr target_img_data, int target_img_size, MatchMode mode);

        /* 销毁匹配模板 */
        [DllImport("libmatch.dll")]
        public static extern void release_template_matcher(IntPtr matcher);

        /* 开始匹配 */
        [DllImport("libmatch.dll")]
        public static extern IntPtr template_matcher_compute(IntPtr matcher, IntPtr src_img_data, int src_img_size, float prob_threshold, float nms_threshold);

        /* 获取结果大小 */
        [DllImport("libmatch.dll")]
        public static extern UIntPtr template_matcher_result_size(IntPtr result);

        /* 获取结果 */
        [DllImport("libmatch.dll")]
        public static extern void template_matcher_result_get(IntPtr result, UIntPtr index, ref ObjectEx result_obj);

        /* 释放结果 */
        [DllImport("libmatch.dll")]
        public static extern void release_template_matcher_result(IntPtr result);

        /* 库文件版本号 */
        public static string Version = Marshal.PtrToStringAnsi(libmatch_version());
    }
}
