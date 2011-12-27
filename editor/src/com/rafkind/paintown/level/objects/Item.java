package com.rafkind.paintown.level.objects;

import java.awt.*;
import java.awt.image.*;

import java.io.File;
import java.io.IOException;

import com.rafkind.paintown.exception.LoadException;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.level.PropertyEditor;
import com.rafkind.paintown.level.ItemEditor;
import com.rafkind.paintown.TokenReader;
import com.rafkind.paintown.MaskedImage;
import com.rafkind.paintown.level.Editor;

public class Item extends Thing {

	private Stimulation stimulation;

	public Item( Token token ) throws LoadException {
		super( token );
		Token name = token.findToken( "name" );
		if ( name != null ){
			setName( name.readString( 0 ) );
		}
		Token stim = token.findToken( "stimulation" );
		if ( stim != null ){
			stimulation = Stimulation.load( stim );
		}
	}

	public Item( Item copy ){
		super( copy );
		if ( copy.stimulation != null ){
			stimulation = copy.stimulation.copy();
		}
	}

	public Stimulation getStimulation(){
		return stimulation;
	}

	public void setStimulation( Stimulation s ){
		stimulation = s;
	}

	protected BufferedImage readIdleImage( String file ) throws LoadException {
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
					return MaskedImage.load( Editor.dataPath( base + pic ) );
				} catch ( IOException ie ){
					throw new LoadException( "Could not load " + base + pic + " at line " + frame.getLine(), ie );
				}
			}
		}
		throw new LoadException( "No idle animation given for " + file );
	}
	
	public Thing copy(){
		return new Item( this );
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
		thing.addToken( new String[]{"id", String.valueOf(getId())} );
		thing.addToken( new String[]{ "name", "\"" + getName() + "\"" } );
		thing.addToken( new String[]{ "type", getType() } );
		thing.addToken( new String[]{ "path", getPath() } );
		thing.addToken( new String[]{ "coords", String.valueOf( getX() ), String.valueOf( getY() ) } );
		if ( stimulation != null ){
			thing.addToken( stimulation.toToken() );
		}

		return thing;
	}
}
