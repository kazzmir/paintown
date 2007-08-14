package com.rafkind.paintown.animator;

import java.util.*;
import javax.swing.*;
import java.awt.*;


public class BoundingBox
{
	// Basic character info
	public int x1;
	public int y1;
	public int x2;
	public int y2;
	
	public boolean empty()
	{
	 			 return (x1 == 0 && y1 == 0 && x2 == 0 && y2 == 0);
	}
	
	public void render(Graphics g)
	{
	 			 Color color = g.getColor();
	 			 
	 			 g.setColor(new Color(255,255,255));
	 			 g.drawRect(x1,y1,x2-x1,y2-y1);
	 			 g.setColor(color);
	}
	
	public BoundingBox(int _x1, int _y1, int _x2, int _y2)
	{
 	 			 x1 = _x1;
 	 			 y1 = _y1;
 	 			 x2 = _x2;
 	 			 y2 = _y2;
	}
}
