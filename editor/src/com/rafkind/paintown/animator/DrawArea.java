package com.rafkind.paintown.animator;

import java.util.*;
import javax.swing.*;
import java.awt.*;
import javax.swing.*;
import java.awt.image.*;
import java.awt.geom.*;
import java.awt.event.*;
import javax.swing.Timer;
import javax.swing.event.*;

import org.swixml.SwingEngine;

import com.rafkind.paintown.animator.*;
import com.rafkind.paintown.*;

public final class DrawArea extends JComponent
{
	private BufferedImage img;
	private int img_x;
	private int img_y;
	private int offset_x;
	private int offset_y;
	private static int x=320;
	private static int y=200;
	private static int _delay=160;
	private int delay;
	private boolean _delayChanged;
	private BoundingBox attackArea = new BoundingBox(0,0,0,0);

	public Dimension getPreferredSize()
	{
		return new Dimension(640,480);
	}

	public void paintComponent( Graphics g ){

		g.setColor( new Color( 0, 0, 0 ) );
		g.fillRect( 0, 0, 640, 480 );
		g.setColor( new Color( 255, 255, 0 ) );
		g.drawLine(0,350,640,350);
		if(img != null)
			g.drawImage( img, x + img_x + offset_x, y + img_y + offset_y, null );
		if(!attackArea.empty())
			attackArea.render(g, x + img_x + offset_x, y + img_y + offset_y);
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
		img_x += x;
	}
	
	public void setImageY(int y)
	{
		img_y += y;
	}
	
	public void setOffsetX(int x)
	{
		offset_x = x;
	}
	
	public void setOffsetY(int y)
	{
		offset_y = y;
	}
	
	public void setAttack(BoundingBox at)
	{
		attackArea = at;
	}
	
	public void resetData()
	{
		img_x = img_y = offset_x = offset_y = 0;
		attackArea = new BoundingBox(0,0,0,0);
	}
	
	public void setDelay(int d)
	{
		delay = d * 100;
		_delayChanged=true;
	}
	
	public int getDelay()
	{
		return delay;
	}
	
	public void resetDelay()
	{
		delay = _delay;
		_delayChanged = true;
	}
	
	public boolean delayChanged()
	{
		if(_delayChanged)
		{
			_delayChanged=false;
			return true;
		}
		return _delayChanged;
	}
	
	public DrawArea()
	{
		img = null; 
		_delayChanged = false;
		/*
		Action doAnim = new AbstractAction() {
			public void actionPerformed(ActionEvent e) {	
				repaint();
			}
		};
	
		new Timer(1000/60,doAnim).start();
		*/
	}
}
