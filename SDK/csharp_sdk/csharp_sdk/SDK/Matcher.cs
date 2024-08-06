using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LibMatch
{

    public class TemplateMatcher : IDisposable
    {

        IntPtr Matcher = IntPtr.Zero;

        public TemplateMatcher(byte[] target_image, MatchMode mode)
        {
            Matcher = NativeApi.create_template_matcher(Utils.BytesToPtr(target_image), target_image.Length, mode);
        }

        public List<ObjectEx> Compute(byte[] source_image, float prob_threshold = 0.65f, float nms_threshold = 0.25f)
        {
            if (Matcher == IntPtr.Zero)
                throw new Exception("Matcher == IntPtr.Zero");

            IntPtr result = NativeApi.template_matcher_compute(Matcher, Utils.BytesToPtr(source_image), source_image.Length, prob_threshold, nms_threshold);
            UInt64 size = NativeApi.template_matcher_result_size(result).ToUInt64();

            List<ObjectEx> res = new List<ObjectEx>();

            for (UInt64 i = 0; i < size; i++) {
                ObjectEx obj = new ObjectEx();
                NativeApi.template_matcher_result_get(result, new UIntPtr(i), ref obj);
                res.Add(obj);
            }

            NativeApi.release_template_matcher_result(result);

            return res;
        }

        ~TemplateMatcher()
        {
            Dispose();
        }

        public void Dispose()
        {
            if (Matcher != IntPtr.Zero)
            {
                NativeApi.release_template_matcher(Matcher);
                Matcher = IntPtr.Zero;
            }
        }
    }
}
