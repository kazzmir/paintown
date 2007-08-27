package com.rafkind.paintown.animator;

import java.io.File;

import java.util.Iterator;

import com.rafkind.paintown.Token;
import com.rafkind.paintown.TokenReader;

import com.rafkind.paintown.exception.LoadException;

public class Projectile extends AnimatedObject {

	private Animation main;
	private Animation death;

	public Projectile( String name ){
		super( name );

		main = new Animation();
		new Thread( main ).start();
		death = new Animation();
		new Thread( death ).start();

		addAnimation( main );
		addAnimation( death );
	}

	public Projectile( String name, File f ) throws LoadException {
		super( name );
		loadData( f );
	}
	
	public void saveData() throws LoadException {
	}

	public Animation getMain(){
		return main;
	}

	public Animation getDeath(){
		return death;
	}

	public void loadData( File f ) throws LoadException {
		TokenReader reader = new TokenReader( f );
		Token head = reader.nextToken();
		
		if ( ! head.getName().equals( "projectile" ) ){
			throw new LoadException( "Starting token is not 'projectile'" );
		}

		Token nameToken = head.findToken( "name" );
		if ( nameToken != null ){
			setName( nameToken.readString(0) );
		}

		for ( Iterator it = head.findTokens( "anim" ).iterator(); it.hasNext(); ){
			Token animation = (Token) it.next();
			Token name = animation.findToken( "name" );
			if ( name != null ){
				if ( name.readString( 0 ).equals( "main" ) ){
					main = new Animation( animation );
					addAnimation( main );
					new Thread( main ).start();
				} else if ( name.readString( 0 ).equals( "death" ) ){
					death = new Animation( animation );
					addAnimation( death );
					new Thread( death ).start();
				} else {
					System.out.println( "Unknown animation" );
					System.out.println( animation.toString() );
				}
			}
		}
	}
}
