package com.rafkind.paintown.level;

import java.awt.*;

import java.io.File;
import java.io.IOException;

import com.rafkind.paintown.exception.LoadException;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.PropertyEditor;
import com.rafkind.paintown.ItemEditor;
import com.rafkind.paintown.TokenReader;
import com.rafkind.paintown.MaskedImage;

public class Item extends Thing {

	public Item( Token token ) throws LoadException {
		super( token );
		Token name = token.findToken( "name" );
		if ( name != null ){
			setName( name.readString( 0 ) );
		}
	}

	protected Image readIdleImage( String file ) throws LoadException {
		TokenReader reader = new TokenReader( new File( file ) );
		Token head = reader.nextToken();
		Token idle = head;
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
	
	protected String getType(){
		return "item";
	}

	public PropertyEditor getEditor(){
		return new ItemEditor( this );
	}

	public Token toToken(){
		Token thing = new Token();
		thing.addToken( new Token( "object" ) );
		thing.addToken( new String[]{ "type", getType() } );
		thing.addToken( new String[]{ "path", getPath() } );
		thing.addToken( new String[]{ "coords", String.valueOf( getX() ), String.valueOf( getY() ) } );

		return thing;
	}
}
