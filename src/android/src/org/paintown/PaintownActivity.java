package org.paintown;

/*
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
            loadAllegroDebug();
        }
    }

    /* load allegro */
    static {
      System.loadLibrary("OpenSLES");
      loadAllegro();
      // System.loadLibrary("paintown");
   }

   public PaintownActivity(){
       super("libpaintown.so");
   }
}
