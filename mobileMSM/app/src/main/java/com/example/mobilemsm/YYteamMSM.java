package com.example.mobilemsm;

import android.util.Log;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;

public class YYteamMSM {

    String TAG="YYteamMSM debug info:";

    private static native String readResultTime(final String dir);

    // Was implemented in libcelo_zprize.so, please ignore the error from IDE.
    private static native void genMSMRandom(final String iters, final String numElems, final String numVecs);

    public String runMSMRandomMultipleVecs(String dir, String iters, String numElems, String numVecs) {
        genMSMRandom(iters, numElems, numVecs);
        try {
            Runtime runtime = Runtime.getRuntime();
            String[] cmdarry = {"./yyteam-mobileMSM", iters};
            File workDir = new File(dir);
            Process process = runtime.exec(cmdarry, null, workDir);
            process.waitFor();

        } catch (IOException | InterruptedException e) {
            Log.i(TAG,e.toString());
        }
        return readResultTime(dir);

    }

    public String runMSMFile(String dir, String iters) {
        try {
            Runtime runtime = Runtime.getRuntime();
            String[] cmdarry = {"./yyteam-mobileMSM", iters};
            File workDir = new File(dir);
            Process process = runtime.exec(cmdarry, null, workDir);
            process.waitFor();

        } catch (IOException | InterruptedException e) {
            Log.i(TAG,e.toString());
        }

        return readResultTime(dir);
    }


}

