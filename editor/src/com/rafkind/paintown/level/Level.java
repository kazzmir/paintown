package com.rafkind.paintown.level;

import java.awt.*;
import java.io.*;
import javax.imageio.*;

import java.util.Iterator;
import java.util.List;
import com.rafkind.paintown.exception.LoadException;
import com.rafkind.paintown.TokenReader;
import com.rafkind.paintown.Token;

public class Level{

	private String name;
	private Image background;
	private int width;

	public Level(){
		this.width = 640;
	}

	public void render( Graphics2D g, int x, int y, int width, int height ){
		g.clearRect( 0, 0, getWidth(), getHeight() );
		/*
		g.setColor( new Color( 255, 0, 0 ) );
		g.drawString( this.name, 20, 20 );
		g.setColor( new Color( 0, 255, 0 ) );
		g.fillOval( 50, 50, 50, 50 );
		*/

		g.scale( 2, 2 );
		if ( background != null ){
			int w = 0;
			while ( w < getWidth() ){
				g.drawImage( background, w, 0, null );
				w += background.getWidth( null );
			}
		}

		/*
		g.setColor( new Color( 0, 255, 0 ) );
		g.fillOval( 50, 50, 50, 50 );
		*/
	}

	public void load( File f ) throws LoadException {
		TokenReader reader = new TokenReader( f );
		Token head = reader.nextToken();
		if ( ! head.getName().equals( "level" ) ){
			throw new LoadException( "Starting token is not 'level'" );
		}

		loadBackground( head.findToken( "background" ) );

		setWidth( calculateWidth( head.findTokens( "block/length" ) ) );
	}

	private void loadBackground( Token t ) throws LoadException {
		if ( t != null ){
			String s = String.valueOf( t.iterator().next() );
			try{
				setBackground( ImageIO.read( new File( s ) ) );
			} catch ( IOException ie ){
				throw new LoadException( "Could not load " + s );
			}
		}
	}

	private void setWidth( int w ){
		width = w;
	}

	private int calculateWidth( List lengths ){
		int w = 0;
		for ( Iterator it = lengths.iterator(); it.hasNext(); ){
			Token t = (Token) it.next();
			w += t.readInt( 0 );
		}
		return w;
	}
	
	private void setBackground( Image i ){
		background = i;
	}

	public Dimension getSize(){
		return new Dimension( getWidth(), getHeight() );
	}

	private int getWidth(){
		return width;
	}

	private int getHeight(){
		return 480;
	}
}
