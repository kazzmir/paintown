package com.rafkind.paintown.animator;

import java.util.*;
import javax.swing.*;
import java.awt.*;
import javax.swing.*;
import java.awt.image.*;
import java.awt.geom.*;

import org.swixml.SwingEngine;

import com.rafkind.paintown.animator.CharacterStats;
import com.rafkind.paintown.animator.DrawState;
import com.rafkind.paintown.Token;

public final class DrawArea extends Canvas
{
	private BufferedImage img;
	private int img_x;
	private int img_y;
	
	public Dimension getPreferredSize()
	{
		return new Dimension(640,480);
	}

	public void paint( Graphics g ){
		if(DrawState.isDrawEnabled() != false)
		{
			g.setColor( new Color( 0, 0, 0 ) );
			g.fillRect( 0, 0, 640, 480 );
			g.setColor( new Color( 255, 255, 0 ) );
			g.drawLine(0,350,640,350);
			if(img != null)g.drawImage( img, img_x, img_y, null );
		}
		else System.out.println( "No drawing!" );
	}
	
	public void setImage(BufferedImage i)
	{
		img = i;
	}
	
	public BufferedImage getImage()
	{
		return img;
	}
	
	public void setImageX(int x)
	{
		img_x = x;
	}
	
	public void setImageY(int y)
	{
		img_y = y;
	}
	
	public int getImageX()
	{
		return img_x;
	}
	
	public int getImageY()
	{
		return img_y;
	}
	
	public DrawArea()
	{
		// Nothing
		//img = new BufferedImage(60,60,BufferedImage.TYPE_INT_ARGB);
		img = null;
	}
}
