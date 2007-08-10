package com.rafkind.paintown.animator;

import java.util.*;
import javax.swing.*;
import java.awt.*;
import javax.swing.*;

import org.swixml.SwingEngine;

import com.rafkind.paintown.animator.CharacterStats;
import com.rafkind.paintown.animator.DrawState;
import com.rafkind.paintown.Token;

public final class DrawArea extends Canvas
{
	private BufferedImage;
	
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
		}
		else System.out.println( "No drawing!" );
	}
	
	public DrawArea()
	{
		// Nothing
	}
}
