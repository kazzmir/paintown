package com.rafkind.paintown.level;

import com.rafkind.paintown.exception.LoadException;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.TokenReader;
import com.rafkind.paintown.MaskedImage;
import com.rafkind.paintown.PropertyEditor;
import com.rafkind.paintown.Lambda2;

import java.io.*;
import java.awt.*;
import java.awt.image.*;
import java.awt.geom.*;
import javax.imageio.*;

import java.util.HashMap;
import java.util.Iterator;
import java.util.ArrayList;
import java.util.List;

public abstract class Thing{

	private int x, y;
	private int width, height;
	private String path;
	private BufferedImage main;
	private String name;
	private boolean selected;
	private static HashMap images = new HashMap();

	private List positionListeners;

	public Thing( Token token ) throws LoadException {
		Token coords = token.findToken( "coords" );
		if ( coords != null ){
			x = coords.readInt( 0 );
			y = coords.readInt( 1 );
		}

		Token tpath = token.findToken( "path" );
		if ( tpath != null ){
			path = tpath.readString( 0 );
			main = loadImage( path, this );
		}

		positionListeners = new ArrayList();
	}

	public BufferedImage getMain(){
		return main;
	}

	public void setName( String s ){
		this.name = s;
	}

	public String getName(){
		return name;
	}

	public String toString(){
		return "X: " + getX() + " Y: " + getY() + " Name: " + getName();
	}

	public String getPath(){
		return path.replaceAll( "\\\\", "/" );
	}

	public abstract PropertyEditor getEditor();

	public int getY(){
		return y;
	}

	public int getX(){
		return x;
	}

	public void setY( int y ){
		// this.y = y + main.getHeight( null ) / 2;
		this.y = y;
		fireUpdatePosition();
	}

	public void setX( int x ){
		this.x = x;
		fireUpdatePosition();
	}

	private void fireUpdatePosition(){
		Integer ix = new Integer( getX() );
		Integer iy = new Integer( getY() );
		for ( Iterator it = positionListeners.iterator(); it.hasNext(); ){
			Lambda2 proc = (Lambda2) it.next();
			proc.invoke_( ix, iy );
		}
	}

	public void addPositionListener( Lambda2 proc ){
		positionListeners.add( proc );
	}

	public void removePositionListener( Lambda2 proc ){
		positionListeners.remove( proc );
	}

	public int getWidth(){
		return main.getWidth( null );
	}

	public int getX1(){
		return getX() - main.getWidth( null ) / 2;
	}

	public int getY1(){
		return getY() - main.getHeight( null );
	}

	public int getX2(){
		return getX() + main.getWidth( null ) / 2;
	}

	public int getY2(){
		return getY();
	}

	public void setSelected( boolean s ){
		selected = s;
	}

	protected void render( Image look, Graphics2D g, boolean highlight ){
		// g.drawImage( main, startX + x, y, null );
		// int mx = startX + x - main.getWidth( null ) / 2;
		// int mx = startX + x + main.getWidth( null ) / 2;
		int mx = x + look.getWidth( null ) / 2;
		int my = y - look.getHeight( null );
		g.drawImage( look, new AffineTransform( -1, 0, 0, 1, mx, my ), null );
		if ( selected ){
			g.setColor( new Color( 0x66, 0xff, 0x77 ) );
		} else if ( highlight ){
			// g.setColor( new Color( 0x66, 0xff, 0x22 ) );
			g.setColor( new Color( 0xff, 0x44, 0x33 ) );
		} else {
			g.setColor( new Color( 64, 64, 255 ) );
		}
		g.drawRect( getX1(), getY1(), look.getWidth( null ), look.getHeight( null ) );
		g.setColor( new Color( 255, 255, 255 ) );
		g.fillOval( x, y, 5, 5 );

	}

	public void render( Graphics2D g, boolean highlight ){
		render( getMain(), g, highlight );
	}

	private static BufferedImage loadImage( String file, Thing t ) throws LoadException {

		if ( images.get( file ) != null ){
			return (BufferedImage) images.get( file );
		}
		images.put( file, t.readIdleImage( file ) );
		return loadImage( file, t );
	}

	protected abstract BufferedImage readIdleImage( String file ) throws LoadException;
	protected abstract String getType();
	public abstract Token toToken();
	
	public boolean equals( Object t ){
		return this == t;
	}
}
