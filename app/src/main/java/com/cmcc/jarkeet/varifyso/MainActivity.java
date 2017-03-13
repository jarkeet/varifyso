package com.cmcc.jarkeet.varifyso;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "MainActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());

        Log.i(TAG, "getSignFrom java " + VerifySign.getSignatureMD5(this));
        Log.i(TAG, "getSignFrom native " + VerifySign.getNativeSignature(this));
        Log.i(TAG, "getNativePassword " + VerifySign.getNativePassword());
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

//     Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

}
