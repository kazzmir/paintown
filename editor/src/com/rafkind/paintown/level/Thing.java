package com.rafkind.paintown.level;

import com.rafkind.paintown.exception.LoadException;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.TokenReader;

import java.io.*;
import java.awt.*;
import javax.imageio.*;

import java.util.HashMap;
import java.util.Iterator;
import java.util.List;

public class Thing{

	private int x, y;
	private Image main;
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
	}

	public void render( Graphics2D g, int startX ){
		g.drawImage( main, startX + x, y, null );
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
					return ImageIO.read( new File( base + pic ) );
				} catch ( IOException ie ){
					throw new LoadException( "Could not load " + base + pic, ie );
				}
			}
		}
		throw new LoadException( "No idle animation given for " + file );
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
