package com.libmatch.sdk;

import com.sun.jna.Pointer;
import com.sun.jna.Structure;

@Structure.FieldOrder({"rect", "prob"})
public class ObjectEx extends Structure {

    public ObjectEx() { super(); }

    public ObjectEx(Pointer pointer) { super(pointer); }

    public Rect rect;

    public float prob;

    public static class ByReference extends ObjectEx implements Structure.ByReference {}

    public static class ByValue extends ObjectEx implements Structure.ByValue {}
}
