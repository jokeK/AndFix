package com.mik.andfix;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private TextView tv;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        tv = (TextView) findViewById(R.id.result);
        DexManager.getInstance().setContext(this);
    }

    public void calculate(View view) {
        Mistake mistake = new Mistake();
        int i = mistake.catchMistake();
        tv.setText(String.valueOf(i));
    }


    public void fix(View view) {
        DexManager.getInstance().loadFile(new File(Environment.getExternalStorageDirectory(),"out.dex"));
    }
}
