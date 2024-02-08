package com.rafkind.paintown;

import java.io.File;

public class Data{
    private static File dataPath = new File( "data" );

    private Data(){
    }

    public static File getDataPath(){
        return dataPath;
    }

    public static void setDataPath(File file){
        dataPath = file;
    }
}
