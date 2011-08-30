package com.paintown;

import java.io.*;
import android.content.res.AssetManager;

public class Paintown extends Activity {

    static AssetManager assetManager;
    
    protected void runCommand(String command){
        try {
            // Executes the command.
            Process process = Runtime.getRuntime().exec(command);
        } catch (IOException e) {
            throw new RuntimeException(e);
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
    }
    
    /** Load assets. */
    @Override
    protected void onCreate(Bundle icicle) {
        
        assetManager = getAssets();
        InputStream in = assetManager.open("paintown.so");
        OutputStream out = new FileOutputStream("/data/data/paintown/com.paintown");
        byte[] buffer = new byte[1024];
        int length;
        while ((length = in.read(buffer)) > 0){
            out.write(buf, 0, len);
        }
        in.close();
        out.close();
        try {
            runCommand("/system/bin/chmod 744 /data/data/paintown/com.paintown");
            // Run paintown
            runCommand("/data/data/paintown/com.paintown");
        } catch (RuntimeException e){
        }
    }
}