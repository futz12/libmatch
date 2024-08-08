package com.libmatch.sdk;

import com.sun.jna.Memory;
import com.sun.jna.Pointer;

public class Utils {
    public static Pointer BytesToPointer(byte[] bytes) {
        Pointer ptr = new Memory(bytes.length);
        ptr.write(0, bytes, 0, bytes.length);
        return ptr;
    }
}
