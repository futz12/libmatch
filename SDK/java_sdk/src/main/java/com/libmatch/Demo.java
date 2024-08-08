package com.libmatch;

import com.libmatch.sdk.*;

import java.io.*;
import java.util.List;

public class Demo {

    public static byte[] readFileByBytes(String fileName) throws IOException {
        try (InputStream in = new BufferedInputStream(new FileInputStream(fileName));
             ByteArrayOutputStream out = new ByteArrayOutputStream();) {
            byte[] tempbytes = new byte[in.available()];
            for (int i = 0; (i = in.read(tempbytes)) != -1;) {
                out.write(tempbytes, 0, i);
            }
            return out.toByteArray();
        }
    }

    public static void main(String[] args) throws IOException {

        System.out.printf("Version: %s\n", NativeApi.Version);
        byte[] img = readFileByBytes("E:\\Project\\libmatch\\SDK\\java_sdk\\eye.png");
        byte[] img2 = readFileByBytes("E:\\Project\\libmatch\\SDK\\java_sdk\\main.png");
        TemplateMatcher matcher = new TemplateMatcher(img, TemplateMatcher.MatchMode.COLOR_BGR);
        List<ObjectEx> res = matcher.Compute(img2);
        for (int i = 0; i < res.size(); i++){
            ObjectEx obj = res.get(i);
            System.out.printf("--------------------------\nprob: %f\nrect:\n    x: %d\n    y: %d\n    width: %d\n    height: %d\n--------------------------\n", obj.prob, obj.rect.x, obj.rect.y, obj.rect.width, obj.rect.height);
        }
    }
}