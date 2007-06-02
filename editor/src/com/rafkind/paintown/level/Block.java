package com.rafkind.paintown.level;

import com.rafkind.paintown.exception.LoadException;
import com.rafkind.paintown.Token;

import java.awt.*;
import java.util.Iterator;
import java.util.List;
import java.util.ArrayList;

public class Block{

	private int length;
	private List objects;

	public Block( Token token ) throws LoadException {
		Token l = token.findToken( "length" );
		if ( l != null ){
			length = l.readInt( 0 );	
		}

		objects = new ArrayList();
		for ( Iterator it = token.findTokens( "object" ).iterator(); it.hasNext(); ){
			Token t = (Token) it.next();
			objects.add( new Thing( t ) );
		}
	}

	public void render( Graphics2D g, int x ){
		for ( Iterator it = this.objects.iterator(); it.hasNext(); ){
			Thing t = (Thing) it.next();
			t.render( g, x );
		}
	}

	public int getLength(){
		return length;
	}

}
