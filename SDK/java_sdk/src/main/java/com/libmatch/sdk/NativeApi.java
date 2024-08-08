package com.libmatch.sdk;

import com.sun.jna.*;

public interface NativeApi extends Library {

    NativeApi Instance = (NativeApi) Native.loadLibrary("libmatch.dll" , NativeApi.class);

    String Version = Instance.libmatch_version();

    String libmatch_version();

    Pointer create_template_matcher(Pointer target_image, int target_img_size, int mode);

    void release_template_matcher(Pointer matcher);

    Pointer template_matcher_compute(Pointer matcher, Pointer src_img_data, int src_img_size, float prob_threshold, float nms_threshold);

    int template_matcher_result_size(Pointer result);

    void template_matcher_result_get(Pointer result, int idx, ObjectEx.ByReference object);

    void release_template_matcher_result(Pointer result);

}
