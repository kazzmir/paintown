package com.rafkind.paintown.animator;

import java.util.*;
import java.io.*;

public class DrawState
{
	private static boolean drawingEnabled;
	
	private static Vector currentDirectoryList;
	
	public static boolean isDrawEnabled()
	{
		return drawingEnabled;
	}
	
	public static void setDrawEnabled(boolean d)
	{
		drawingEnabled = d;
	}
	
	public static void setCurrentDirList(Vector l)
	{
		currentDirectoryList = l;
	}
	
	public static void setCurrentDirList(String dir)
	{
		// Directory List
		File file = new File(dir);
		if ( file.isDirectory() ){
			currentDirectoryList = new Vector();
			File[] all = file.listFiles();
			//files.add( new File( ".." ) );
			for ( int i = 0; i < all.length; i++ ){
				currentDirectoryList.addElement( dir + all[ i ].getName() );
			}
		}
	}
	
	public static Vector getCurrentDirList()
	{
		return currentDirectoryList;
	}
	
}
