package com.rafkind.paintown.animator;

import java.util.*;

public class DrawState
{
	private static boolean drawingEnabled = true;;
	public DrawState()
	{
		// Nothing
	}
	
	public static boolean isDrawEnabled()
	{
		return drawingEnabled;
	}
	
	public static void setDrawEnabled(boolean d)
	{
		drawingEnabled = d;
	}
}
