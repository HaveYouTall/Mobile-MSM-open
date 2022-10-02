package com.example.mobilemsm;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.text.InputType;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class MainActivity extends AppCompatActivity {

    String TAG="Main debug info:";
    private final Handler mainThreadHandler = Handler.createAsync(Looper.getMainLooper());

    static {
        System.loadLibrary("mobilemsm"); // Load for catch the result time.
        System.loadLibrary("celo_zprize"); // Load for generate random vectors.
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        LinearLayout linearLayout = findViewById(R.id.rootContainer);

        Button btnShow = new Button(this);
        btnShow.setText("Press to run using random elements");
        btnShow.setLayoutParams(new LinearLayout.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT));

        Button btnShowFile = new Button(this);
        btnShowFile.setText("Press to run from test vector file");
        btnShowFile.setLayoutParams(new LinearLayout.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT));

        EditText iters = new EditText(this);
        iters.setHint("#iterations per vector");
        iters.setInputType(InputType.TYPE_CLASS_NUMBER);

        EditText numElems = new EditText(this);
        numElems.setHint("#elems as power of 2");
        numElems.setInputType(InputType.TYPE_CLASS_NUMBER);

        EditText numVecs = new EditText(this);
        numVecs.setHint("#vectors to generate randomly");
        numVecs.setInputType(InputType.TYPE_CLASS_NUMBER);

        TextView resultView = new TextView(this);
        TextView resultView2 = new TextView(this);

        File filePoints = new File(getFilesDir()+"/points");
        try {

            InputStream is = getAssets().open("points");
            int size = is.available();
            byte[] buffer = new byte[size];
            is.read(buffer);
            is.close();

            FileOutputStream fos = new FileOutputStream(filePoints);
            fos.write(buffer);
            fos.close();
        } catch (Exception e) { throw new RuntimeException(e); }

        File fileScalars = new File(getFilesDir()+"/scalars");
        try {

            InputStream is = getAssets().open("scalars");
            int size = is.available();
            byte[] buffer = new byte[size];
            is.read(buffer);
            is.close();

            FileOutputStream fos = new FileOutputStream(fileScalars);
            fos.write(buffer);
            fos.close();
        } catch (Exception e) { throw new RuntimeException(e); }


        try {
            String[] cmdarry = {"chmod", "777", "/data/data/com.example.mobilemsm/files/"};
            Process process = Runtime.getRuntime().exec(cmdarry, null, null);
            process.waitFor();

            String[] cmdarry2 = {"chmod", "666", "/data/data/com.example.mobilemsm/files/points"};
            process = Runtime.getRuntime().exec(cmdarry2, null, null);
            process.waitFor();

            String[] cmdarry3 = {"chmod", "666", "/data/data/com.example.mobilemsm/files/scalars"};
            process = Runtime.getRuntime().exec(cmdarry3, null, null);
            process.waitFor();

        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }

        // Set up our library.
        try {
            // Remove previous symlink cache
            String[] cmdarry = {"rm", "/data/data/com.example.mobilemsm/files/yyteam-mobileMSM"};
            Process process = Runtime.getRuntime().exec(cmdarry, null, null);
            process.waitFor();

            // Create a new symlink
            String[] cmdarry2 = {"ln", "-s", getApplicationInfo().nativeLibraryDir + "/libyyteam-mobileMSM.so", "/data/data/com.example.mobilemsm/files/yyteam-mobileMSM"};
            process = Runtime.getRuntime().exec(cmdarry2, null, null);
            process.waitFor();
        } catch (IOException | InterruptedException e) {
            Log.i(TAG,e.toString());
        }


        btnShow.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                resultView.setText("Currently running on random vectors");
                resultView2.setText("");
                File dir = getFilesDir();
                String dir_path = dir.getAbsolutePath();
                String iters_val = iters.getText().toString();
                String numElemsVal = numElems.getText().toString();
                String numVecsVal = numVecs.getText().toString();
                if (TextUtils.isDigitsOnly(iters_val) && !TextUtils.isEmpty(iters_val)
                        && TextUtils.isDigitsOnly(numElemsVal) && !TextUtils.isEmpty(numElemsVal)) {
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            YYteamMSM g = new YYteamMSM();
                            Log.i(TAG, "Starting MSM with random inputs");
                            String r = g.runMSMRandomMultipleVecs(dir_path, iters_val, numElemsVal, numVecsVal);
                            Log.i(TAG, "Finished MSM with random inputs");
                            mainThreadHandler.post(new Runnable() {
                                @Override
                                public void run() {
                                    String result = "Mean time to run with random elements is: ";
                                    resultView.setText(result);
                                    resultView2.setText(r);
                                }
                            });
                        }
                    }).start();
                } else {
                    resultView.setText("Valid number of iterations, vectors, and elements per vector must be provided");
                    resultView2.setText("");
                }
            }
        });

        btnShowFile.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                resultView.setText("Currently running on test vector file");
                resultView2.setText("");
                YYteamMSM g = new YYteamMSM();
                File dir = getFilesDir();
                String dir_path = dir.getAbsolutePath();
                String iters_val = iters.getText().toString();
                if (TextUtils.isDigitsOnly(iters_val) && !TextUtils.isEmpty(iters_val)) {
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            YYteamMSM g = new YYteamMSM();
                            Log.i(TAG, "Starting MSM with test vector file");
                            String r = g.runMSMFile(dir_path, iters_val);
                            Log.i(TAG, "Finished MSM with test vector file");
                            mainThreadHandler.post(new Runnable() {
                                @Override
                                public void run() {
                                    String result = "Mean time to run with test vector file is: ";
                                    resultView.setText(result);
                                    resultView2.setText(r);
                                }
                            });
                        }
                    }).start();
                } else {
                    resultView.setText("Valid number of iterations must be provided");
                    resultView2.setText("");
                }

            }
        });

        // Add Button to LinearLayout
        if (linearLayout != null) {
            linearLayout.addView(btnShowFile);
            linearLayout.addView(btnShow);
            linearLayout.addView(iters);
            linearLayout.addView(numVecs);
            linearLayout.addView(numElems);
            linearLayout.addView(resultView);
            linearLayout.addView(resultView2);
        }

    }

}