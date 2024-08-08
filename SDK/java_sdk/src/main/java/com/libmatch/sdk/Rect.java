package com.libmatch.sdk;

import com.sun.jna.Pointer;
import com.sun.jna.Structure;

@Structure.FieldOrder({"x", "y", "width", "height"})
public class Rect extends Structure {

    public Rect() { super(); }

    public Rect(Pointer pointer) { super(pointer); }

    public int x;

    public int y;

    public int width;

    public int height;

    public static class ByReference extends Rect implements Structure.ByReference {}

    public static class ByValue extends Rect implements Structure.ByValue {}
}