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
import com.rafkind.paintown.animator.events.AnimationEvent;
import com.rafkind.paintown.*;

public final class DrawArea extends JComponent
{
	private BufferedImage img;
	private int img_x;
	private int img_y;
	private int offset_x;
	private int offset_y;
	private static int x = 320;
	private static int y = 200;
	private BoundingBox attackArea;
	private long delayTime;
	private long endTime;
	// private Vector registeredEvents;
	private Iterator eventItor;
	private AnimationEvent currentEvent;
	private Animation currentAnimation;

	public DrawArea(){
		/*
		img = null;
		attackArea = new BoundingBox(0,0,0,0);
		delayTime = 1;
		endTime = 1;
		new Timer(1, new AbstractAction() {
			public void actionPerformed(ActionEvent e) {	
				update();
			}
		}).start();
		*/
		currentAnimation = null;
	}


	public Dimension getPreferredSize(){
		return new Dimension(640,480);
	}

	public void paintComponent( Graphics g ){

		g.setColor( new Color( 0, 0, 0 ) );
		g.fillRect( 0, 0, 640, 480 );
		g.setColor( new Color( 255, 255, 0 ) );
		g.drawLine(0,350,640,350);

		if ( currentAnimation != null ){
			currentAnimation.draw( g, x, y );
		}

		/*
		if(img != null)
			g.drawImage( img, x + img_x + offset_x, y + img_y + offset_y, null );
		if(!attackArea.empty())
			attackArea.render(g, x + img_x + offset_x, y + img_y + offset_y);
		*/
	}

	public void animate( Animation animation ){
		unanimate();
		currentAnimation = animation;
		currentAnimation.addDrawable( this );
	}

	public void unanimate(){
		if ( currentAnimation != null ){
			currentAnimation.removeDrawable( this );
		}
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
		if(d != 0)d = 1;
		else delayTime = 9000000/d;
	}
	
	private void startDelay()
	{
		endTime = Calendar.getInstance().getTimeInMillis() + delayTime;
	}
	
	public boolean checkDelay()
	{
		return (Calendar.getInstance().getTimeInMillis() > endTime);
	}
	
	/*
	public void registerEvents(Vector events)
	{
		registeredEvents = events;
		if(registeredEvents != null)
			eventItor = registeredEvents.iterator();
		else resetEvents();
	}
	*/
	
	/*
	public void resetEvents()
	{
		registeredEvents = null;
		eventItor = null;
		currentEvent = null;
	}
	*/
	
	public AnimationEvent getCurrentEvent()
	{
		return currentEvent;
	}
	
	/*
	private void update()
	{
		if(registeredEvents!=null)
		{
			if(checkDelay())
			{
				if(eventItor.hasNext())
				{
					currentEvent = (AnimationEvent)eventItor.next();
					currentEvent.interact(this);
					repaint();
					if(currentEvent.getToken().getName().equals("frame") && eventItor.hasNext())startDelay();
				}
				else
				{
					eventItor = registeredEvents.iterator();
					resetData();
				}
			}
			else
			{
				System.out.println(Long.toString(Calendar.getInstance().getTimeInMillis()) + " < " + Long.toString(endTime));
			}
		}
	}
	*/
}
