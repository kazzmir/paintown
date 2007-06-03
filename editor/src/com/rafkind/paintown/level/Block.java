package com.rafkind.paintown.level;

import com.rafkind.paintown.exception.LoadException;
import com.rafkind.paintown.Token;

import java.awt.*;
import java.util.Iterator;
import java.util.List;
import java.util.ArrayList;
import java.util.TreeSet;
import java.util.Comparator;

public class Block{

	private int length;
	private TreeSet objects;
	private boolean enabled = true;

	public Block( Token token ) throws LoadException {
		Token l = token.findToken( "length" );
		if ( l != null ){
			length = l.readInt( 0 );	
		}

		objects = new TreeSet( new Comparator(){
			public int compare( Object o1, Object o2 ){
				Thing t1 = (Thing) o1;
				Thing t2 = (Thing) o2;
				if ( t1.getY() < t2.getY() ){
					return -1;
				}
				if ( t1.getY() > t2.getY() ){
					return 1;
				}
				return 0;
			}
			
			public boolean equals( Object o ){
				return false;
			}
		});
		for ( Iterator it = token.findTokens( "object" ).iterator(); it.hasNext(); ){
			Token t = (Token) it.next();
			objects.add( new Thing( t ) );
		}
	}

	public boolean isEnabled(){
		return enabled;
	}

	public void setEnabled( boolean b ){
		enabled = b;
	}

	public void render( Graphics2D g, int x, int height, int minZ, int maxZ ){
		g.translate( 0, minZ );
		for ( Iterator it = this.objects.iterator(); it.hasNext(); ){
			Thing t = (Thing) it.next();
			t.render( g, x );
		}
		g.translate( 0, -minZ );
		g.setColor( new Color( 255, 255, 255 ) );
		g.fillRect( x, 0, 2, height );
	}

	public int getLength(){
		return length;
	}

}
