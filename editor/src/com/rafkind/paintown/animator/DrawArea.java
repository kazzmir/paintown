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

public final class DrawArea extends JComponent {
	private int x = 150;
	private int y = 100;
	private double scale;
	
	private AnimationEvent currentEvent;
	private Animation currentAnimation;

	public DrawArea(){
		currentAnimation = null;

		scale = 1.0;

		this.addMouseMotionListener( new MouseMotionAdapter(){
			public void mouseDragged( MouseEvent event ){
				x = (int)(event.getX() / getScale());
				y = (int)(event.getY() / getScale());
				DrawArea.this.repaint();
			}
		});
	}

	public double getScale(){
		return scale;
	}

	public void setScale( double x ){
		scale = x;
		repaint();
	}

	public Dimension getPreferredSize(){
		return new Dimension(640,480);
	}

	public void paintComponent( Graphics g ){
		
		Graphics2D g2d = (Graphics2D) g;
		g2d.scale( getScale(), getScale() );

		g.setColor( new Color( 0, 0, 0 ) );
		g.fillRect( 0, 0, 640, 480 );
		g.setColor( new Color( 255, 255, 0 ) );
		g.drawLine( 0, y, 640, y );
		g.drawLine( x, 0, x, 480 );

		if ( currentAnimation != null ){
			currentAnimation.draw( g, x, y - currentAnimation.getHeight() );
		}
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
	
	public AnimationEvent getCurrentEvent(){
		return currentEvent;
	}
}
