package com.libmatch.sdk;

import com.sun.jna.IntegerType;
import com.sun.jna.Pointer;

import java.util.ArrayList;
import java.util.List;

public class TemplateMatcher {

    public enum MatchMode {
        COLOR_GRAY,
        COLOR_BGRA,
        COLOR_BGR,
        COLOR_BGRA_COLOR,
        COLOR_BGR_MASK,
        COLOR_GRAY_MASK,
    }

    protected Pointer matcher;

    public TemplateMatcher(byte[] target_image, MatchMode mode) {
        matcher = NativeApi.Instance.create_template_matcher(Utils.BytesToPointer(target_image), target_image.length, mode.ordinal());
    }

    public List<ObjectEx> Compute(byte[] source_image) {
        return Compute(source_image, 0.65f, 0.25f);
    }

    public List<ObjectEx> Compute(byte[] source_image, float prob_threshold, float nms_threshold) {
        Pointer result = NativeApi.Instance.template_matcher_compute(matcher, Utils.BytesToPointer(source_image), source_image.length, prob_threshold, nms_threshold);
        int size = NativeApi.Instance.template_matcher_result_size(result);

        List<ObjectEx> list = new ArrayList<>();

        for (int i = 0; i < size; i++){
            ObjectEx.ByReference res = new ObjectEx.ByReference();
            NativeApi.Instance.template_matcher_result_get(result, i, res);
            list.add(res);
        }

        NativeApi.Instance.release_template_matcher_result(result);

        return list;
    }

    protected void finalize() {
        if(matcher != Pointer.NULL)
            NativeApi.Instance.release_template_matcher(matcher);
    }

}
