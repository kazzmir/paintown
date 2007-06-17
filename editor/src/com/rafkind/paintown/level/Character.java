package com.rafkind.paintown.level;

import java.awt.*;
import java.io.*;

import java.util.List;
import java.util.Iterator;
import java.util.HashMap;

import com.rafkind.paintown.exception.LoadException;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.TokenReader;
import com.rafkind.paintown.MaskedImage;
import com.rafkind.paintown.PropertyEditor;
import com.rafkind.paintown.CharacterEditor;
import com.rafkind.paintown.Lambda0;

public class Character extends Thing {

	private HashMap remaps;
	private int health;
	private int map;
	private int maxMaps;
	private int aggression;

	public Character( Token token ) throws LoadException {
		super( token );

		Token alias = token.findToken( "name" );
		if ( alias == null ){
			alias = token.findToken( "alias" );
		}
		if ( alias != null ){
			setName( alias.readString( 0 ) );
		}
		Token remap = token.findToken( "map" );
		if ( remap != null ){
			setMap( remap.readInt( 0 ) );
		} else {
			setMap( 0 );
		}
		Token aggr = token.findToken( "aggression" );
		if ( aggr != null ){
			setAggression( aggr.readInt( 0 ) );
		} else {
			setAggression( -1 );
		}
		maxMaps = calculateMaxMaps( getPath() );

		remaps = new HashMap();

		this.remaps.put( new Integer( 0 ), new Lambda0(){
			public Object invoke(){
				return getMain();
			}
		});
	}

	public void render( Graphics2D g, boolean highlight ){
		Lambda0 proc = (Lambda0) this.remaps.get( new Integer( getMap() ) );
		if ( proc == null ){
			proc = (Lambda0) this.remaps.get( new Integer( 0 ) );
		}
		render( (Image) proc.invoke_(), g, highlight );
	}

	public int getAggression(){
		return aggression;
	}

	public void setAggression( int a ){
		this.aggression = a;
	}
	
	public int getMap(){
		return this.map;
	}

	public void setMap( int m ){
		this.map = m;
	}

	public int getMaxMaps(){
		return maxMaps;
	}

	private int calculateMaxMaps( String file ) throws LoadException {
		TokenReader reader = new TokenReader( new File( file ) );
		Token head = reader.nextToken();
		return head.findTokens( "remap" ).size();
	}

	protected Image readIdleImage( String file ) throws LoadException {
		TokenReader reader = new TokenReader( new File( file ) );
		Token head = reader.nextToken();
		Token idle = null;
		idle = findIdle( head.findTokens( "anim" ) );

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
					throw new LoadException( "Could not load " + base + pic + " at line " + frame.getLine(), ie );
				}
			}
		}
		throw new LoadException( "No idle animation given for " + file );
	}

	public int getHealth(){
		return health;
	}

	public void setHealth( int i ){
		health = i;
	}
	
	private Token findIdle( List tokens ){
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
	
	protected String getType(){
		return "enemy";
	}
	
	public PropertyEditor getEditor(){
		return new CharacterEditor( this );
	}

	public Token toToken(){
		Token thing = new Token();
		thing.addToken( new Token( "object" ) );
		thing.addToken( new String[]{ "type", getType() } );
		thing.addToken( new String[]{ "path", getPath() } );
		thing.addToken( new String[]{ "map", String.valueOf( getMap() ) } );
		if ( getAggression() != -1 ){
			thing.addToken( new String[]{ "aggression", String.valueOf( getAggression() ) } );
		}
		thing.addToken( new String[]{ "coords", String.valueOf( getX() ), String.valueOf( getY() ) } );
		return thing;
	}

}
