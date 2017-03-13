package com.cmcc.jarkeet.varifyso;

import android.content.Context;

/**
 * Created by jeff on 2017/3/10.
 */

public class NativeContext {
    /**
     * App context
     */
    private static Context sAppContext;

    /**
     * 得到 app context
     */
    public static Context getAppContext() {
        return sAppContext;
    }

    /**
     * Set the app context
     */
    public static void setAppContext(Context appContext) {
        sAppContext = appContext;
    }
}
