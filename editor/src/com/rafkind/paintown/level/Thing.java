package com.rafkind.paintown.level;

import com.rafkind.paintown.exception.LoadException;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.TokenReader;
import com.rafkind.paintown.MaskedImage;

import java.io.*;
import java.awt.*;
import java.awt.geom.*;
import javax.imageio.*;

import java.util.HashMap;
import java.util.Iterator;
import java.util.List;

public class Thing{

	private int x, y;
	private int width, height;
	private Image main;
	private String name;
	private static HashMap images = new HashMap();

	public Thing( Token token ) throws LoadException {
		Token coords = token.findToken( "coords" );
		if ( coords != null ){
			x = coords.readInt( 0 );
			y = coords.readInt( 1 );
		}
		Token path = token.findToken( "path" );
		if ( path != null ){
			String file = path.readString( 0 );
			main = loadIdleImage( file );
		}

		Token alias = token.findToken( "name" );
		if ( alias == null ){
			alias = token.findToken( "alias" );
		}
		if ( alias != null ){
			name = alias.readString( 0 );	
		}
	}

	public String toString(){
		return "X: " + getX() + " Y: " + getY() + " Name: " + name;
	}

	public int getY(){
		return y;
	}

	public int getX(){
		return x;
	}

	public void setY( int y ){
		// this.y = y + main.getHeight( null ) / 2;
		this.y = y;
	}

	public void setX( int x ){
		this.x = x;
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

	public void render( Graphics2D g, int startX ){
		// g.drawImage( main, startX + x, y, null );
		// int mx = startX + x - main.getWidth( null ) / 2;
		int mx = startX + x + main.getWidth( null ) / 2;
		int my = y - main.getHeight( null );
		g.drawImage( main, new AffineTransform( -1, 0, 0, 1, mx, my ), null );
		g.setColor( new Color( 255, 255, 255 ) );
		g.fillOval( startX + x, y, 5, 5 );
	}

	private static Image loadIdleImage( String file ) throws LoadException {

		if ( images.get( file ) != null ){
			return (Image) images.get( file );
		}
		images.put( file, readIdleImage( file ) );
		return loadIdleImage( file );
	}

	private static Image readIdleImage( String file ) throws LoadException {
		TokenReader reader = new TokenReader( new File( file ) );
		Token head = reader.nextToken();
		Token idle = null;
		if ( head.getName().equals( "character" ) ){
			idle = findIdle( head.findTokens( "anim" ) );
		} else if ( head.getName().equals( "item" ) ){
			idle = head;
		}
		if ( idle != null ){
			String base = "./";
			Token basedir = idle.findToken( "basedir" );
			if ( basedir != null ){
				base = basedir.readString( 0 );
			}
			Token frame = idle.findToken( "frame" );
			if ( frame != null ){
				String pic = frame.readString( 0 );
				try{
					return MaskedImage.load( base + pic );
				} catch ( IOException ie ){
					throw new LoadException( "Could not load " + base + pic, ie );
				}
			}
		}
		throw new LoadException( "No idle animation given for " + file );
	}

	public boolean equals( Object t ){
		return this == t;
	}

	private static Token findIdle( List tokens ){
		for ( Iterator it = tokens.iterator(); it.hasNext(); ){
			Token t = (Token) it.next();
			Token name = t.findToken( "name" );
			if ( name != null ){
				if ( name.readString( 0 ).equals( "idle" ) ){
					return t;
				}
			}
		}
		return null;
	}
}
