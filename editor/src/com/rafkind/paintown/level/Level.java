package com.rafkind.paintown.level;

import java.awt.*;
import java.io.*;

import com.rafkind.paintown.exception.LoadException;
import com.rafkind.paintown.TokenReader;
import com.rafkind.paintown.Token;

public class Level{

	private String name;

	public Level(){
		this.name = "Hellooooo nurse!";
	}

	public void render( Graphics g, int x, int y, int width, int height ){
		g.clearRect( 0, 0, getWidth(), getHeight() );
		g.setColor( new Color( 255, 0, 0 ) );
		g.drawString( this.name, 20, 20 );
		g.setColor( new Color( 0, 255, 0 ) );
		g.fillOval( 50, 50, 50, 50 );
	}

	public void load( File f ) throws LoadException {
		TokenReader reader = new TokenReader( f );
		Token head = reader.nextToken();
		if ( ! head.getName().equals( "level" ) ){
			throw new LoadException( "Starting token is not 'level'" );
		}
	}

	public Dimension getSize(){
		return new Dimension( getWidth(), getHeight() );
	}

	private int getWidth(){
		return 1000;
	}

	private int getHeight(){
		return 480;
	}
}
