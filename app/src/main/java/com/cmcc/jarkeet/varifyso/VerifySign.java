package com.cmcc.jarkeet.varifyso;

import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;


/**
 * Created by jeff on 2017/3/10.
 */

public class VerifySign {

    static {
        System.loadLibrary("verifyso");
    }


    public static String getSignature(Context context) {
        try {

            PackageInfo packageInfo = context.getPackageManager().getPackageInfo(context.getPackageName(), PackageManager.GET_SIGNATURES);

            Signature[] signatures = packageInfo.signatures;

            return signatures[0].toCharsString();

        } catch (Exception e) {
            e.printStackTrace();
        }

        return null;
    }

    public static String getSignMD5(Context context) {
        try {
            MessageDigest messageDigest = MessageDigest.getInstance("MD5");

            return bytesToHexString(messageDigest.digest(getSignature(context).getBytes()));

        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }
        return null;
    }

    public static String bytesToHexString(byte[] src){
        StringBuilder stringBuilder = new StringBuilder("");
        if (src == null || src.length <= 0) {
            return null;
        }
        for (int i = 0; i < src.length; i++) {
            int v = src[i] & 0xFF;
            String hv = Integer.toHexString(v);
            if (hv.length() < 2) {
                stringBuilder.append(0);
            }
            stringBuilder.append(hv+" ");
        }
        return stringBuilder.toString();
    }

    public native String getNativeSignature(Context context);

    public native String getNativePassword();


}
