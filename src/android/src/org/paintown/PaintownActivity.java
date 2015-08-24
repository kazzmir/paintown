package org.paintown;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Environment;

import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.OrientationEventListener;
import android.view.WindowManager;

import android.hardware.*;
import android.content.res.Configuration;
import android.content.res.AssetManager;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.PixelFormat;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.ActivityInfo;
import android.util.Log;

import java.lang.String;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.lang.reflect.InvocationTargetException;
import java.lang.Runnable;

import java.util.List;
import java.util.BitSet;
import java.util.ArrayList;

import java.io.File;
import java.io.InputStream;
import java.io.FileOutputStream;

import android.widget.RelativeLayout;
import android.widget.LinearLayout;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.ProgressBar;

import android.content.res.AssetFileDescriptor;
import java.util.zip.*;
import java.io.File;
import java.io.BufferedOutputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.StringReader;
import java.io.InputStream;

/*
import java.nio.ByteBuffer;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.*;
import android.media.AudioManager;
*/

import org.liballeg.android.AllegroActivity;

public class PaintownActivity extends AllegroActivity {
    static void loadAllegroLibraries(String suffix){
        System.loadLibrary("allegro" + suffix);
        System.loadLibrary("allegro_primitives" + suffix);
        System.loadLibrary("allegro_image" + suffix);
        System.loadLibrary("allegro_memfile" + suffix);
        System.loadLibrary("allegro_font" + suffix);
        System.loadLibrary("allegro_ttf" + suffix);
        System.loadLibrary("allegro_audio" + suffix);
        System.loadLibrary("allegro_acodec" + suffix);
    }

    static void loadAllegroNormal(){
        loadAllegroLibraries("");
    }

    static void loadAllegroDebug(){
        loadAllegroLibraries("-debug");
    }

    static void loadAllegro(){
        try{
            loadAllegroNormal();
        } catch (UnsatisfiedLinkError fail){
			Log.v("Paintown", "Could not load normal allegro libraries: " + fail.getMessage());
			fail.printStackTrace();
			try{
				loadAllegroDebug();
			} catch (UnsatisfiedLinkError fail2){
				Log.v("Paintown", "Could not load allegro debug libraries: " + fail2.getMessage());
				fail2.printStackTrace();
				throw new RuntimeException("Failure to load any libraries");
			}
        }
    }

    /* load allegro */
    static {
      System.loadLibrary("OpenSLES");
      loadAllegro();
   }

   public PaintownActivity(){
       super("libpaintown.so");
   }

   public static String getDataDirectory(){
	   return Environment.getExternalStorageDirectory().getAbsolutePath() + "/paintown";
   }

   /* copy the data bundled in assets to the external data directory */
   private void setupData(final Context context){
	   Log.v("Paintown", "Set up data");
	   final File root = new File(getDataDirectory());
	   File data = new File(root, "data");
	   File installed = new File(data, "installed");
	   Log.v("Paintown", "Data " + data.getPath() + " exists?" + data.exists());
	   if (installed.exists()){
		   return;
	   }

	   /* horizontal progress bar */
	   final ProgressBar progress = new ProgressBar(context, null, android.R.attr.progressBarStyleHorizontal);

	   /* we are in a background thread so to get the bar to show up we
		* need to run it on the main UI thread
		*/
	   runOnUiThread(new Runnable(){
		   public void run(){
			   setContentView(loadingView(context, progress));
		   }
	   });

	   Log.v("Paintown", "Data directory doesn't exist, creating it: " + getDataDirectory());
	   if (!root.mkdirs()){
		   Log.v("Paintown", "Unable to make data directory");
		   return;
	   }

	   /* Whats the point of creating the user directory? */
	   File user = new File(root, "user");
	   if (!user.exists()){
		   user.mkdirs();
	   }

	   unzip(root, "data.zip", context, progress);

	   try{
		   FileOutputStream stream = new FileOutputStream(installed);
		   stream.write(0);
		   stream.close();
	   } catch (IOException fail){
		   Log.v("Paintown", fail.toString());
	   }
   }

   @Override
   public void onCreate(final Bundle savedInstanceState) {
	   super.onCreate(savedInstanceState);

	   setContentView(welcomeView(getApplication()));

	   new Thread(){
		   public void run(){
			   setupData(getApplication());
		   }
	   }.start();
   }

   private View welcomeView(Context context){
	   RelativeLayout layout = new RelativeLayout(context);
	   RelativeLayout.LayoutParams textParams = new RelativeLayout.LayoutParams(
			   RelativeLayout.LayoutParams.WRAP_CONTENT,
			   RelativeLayout.LayoutParams.WRAP_CONTENT);
	   textParams.addRule(RelativeLayout.CENTER_IN_PARENT);

	   TextView text = new TextView(context);
	   text.setId(800);
	   text.setText("Starting Paintown..");
	   layout.addView(text, textParams);
	   return layout;
   }

   private View loadingView(Context context, ProgressBar progress){
	   RelativeLayout layout = new RelativeLayout(context);
	   RelativeLayout.LayoutParams textParams = new RelativeLayout.LayoutParams(
			   RelativeLayout.LayoutParams.WRAP_CONTENT,
			   RelativeLayout.LayoutParams.WRAP_CONTENT);
	   textParams.addRule(RelativeLayout.CENTER_IN_PARENT);

	   TextView text = new TextView(context);
	   text.setId(800);
	   text.setText("Installing Paintown data to " + getDataDirectory() + "/data");
	   layout.addView(text, textParams);

	   RelativeLayout.LayoutParams progressParams = new RelativeLayout.LayoutParams(
			   RelativeLayout.LayoutParams.WRAP_CONTENT,
			   RelativeLayout.LayoutParams.WRAP_CONTENT);
	   progressParams.addRule(RelativeLayout.BELOW, text.getId());
	   progressParams.addRule(RelativeLayout.CENTER_HORIZONTAL);
	   layout.addView(progress, progressParams);
	   return layout;
   }

   /* get the size of the entire zip file */
   private long zipSize(AssetManager assets, String file) throws IOException {
	   AssetFileDescriptor descriptor = assets.openFd(file);
	   long size = descriptor.getLength();
	   descriptor.close();
	   return size;
   }

   private int zipEntries(AssetManager assets, String file) throws IOException {
	   ZipInputStream zip = new ZipInputStream(assets.open(file));
	   ZipEntry entry = zip.getNextEntry();
	   int count = 0;
	   while (entry != null){
		   count += 1;
		   entry = zip.getNextEntry();
	   }
	   zip.close();
	   return count;
   }

   /* unzips a file from assets into the given root directory */
   private void unzip(File root, String file, Context context, final ProgressBar progress){
	   Log.v("Paintown", "Writing data to " + root.getAbsolutePath());
	   try{
		   AssetManager assets = context.getResources().getAssets();
		   progress.setMax((int) zipEntries(assets, file));
		   ZipInputStream zip = new ZipInputStream(assets.open(file));

		   ZipEntry entry = zip.getNextEntry();
		   long count = 0;
		   while (entry != null){
			   String filename = entry.getName();
			   Log.v("Paintown", "Writing entry " + filename);
			   if (entry.isDirectory()){
				   File directory = new File(root, filename);
				   directory.mkdirs();
			   } else {
				   writeFile(new File(root, filename), entry.getSize(), zip);
			   }

			   // count += entry.getSize();
			   count += 1;

			   entry = zip.getNextEntry();
			   final long xcount = count;

			   /* update the progress bar */
			   runOnUiThread(new Runnable(){
				   public void run(){
					   progress.setProgress((int) xcount);
				   }
			   });
		   }
		   zip.close();
	   } catch (IOException fail){
		   Log.v("Paintown", fail.toString());
	   } catch (RuntimeException fail){
		   Log.v("Paintown", fail.toString());
		   throw fail;
	   }
	   Log.v("Paintown", "Wrote data");
   }

   private void writeFile(File what, long size, ZipInputStream stream) throws IOException {
	   byte[] buffer = new byte[1024];
	   int count;
	   BufferedOutputStream output = new BufferedOutputStream(new FileOutputStream(what));
	   while ((count = stream.read(buffer, 0, buffer.length)) != -1){
		   output.write(buffer, 0, count);
	   }
	   output.close();
   }
}
