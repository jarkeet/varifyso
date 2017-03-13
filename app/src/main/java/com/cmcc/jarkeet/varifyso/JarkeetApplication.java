package com.cmcc.jarkeet.varifyso;

import android.app.Application;

/**
 * Created by jeff on 2017/3/10.
 */

public class JarkeetApplication extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        NativeContext.setAppContext(this);
    }
}
