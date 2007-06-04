package com.rafkind.paintown.level;

import com.rafkind.paintown.exception.LoadException;
import com.rafkind.paintown.Token;

import java.awt.*;
import java.util.Iterator;
import java.util.List;
import java.util.ArrayList;
import java.util.TreeSet;
import java.util.Arrays;
import java.util.Comparator;

public class Block{

	private int length;
	private List objects;
	private boolean enabled = true;

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

	/*
	public synchronized void reorder(){
		Object[] a = objects.toArray();
		objects.clear();
		for ( int i = 0; i < a.length; i++ ){
			objects.add( a[ i ] );
		}
		System.out.println( "Reorder: " + objects.size() );
	}
	*/

	public boolean isEnabled(){
		return enabled;
	}

	public void setEnabled( boolean b ){
		enabled = b;
	}

	public synchronized void render( Graphics2D g, int x, int height, int minZ, int maxZ ){
		g.translate( 0, minZ );
		Object[] objs = this.objects.toArray();
		Arrays.sort( objs, new Comparator(){
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
		for ( int i = 0; i < objs.length; i++ ){
			Thing t = (Thing) objs[ i ];
			t.render( g, x );
		}
		g.translate( 0, -minZ );
		g.setColor( new Color( 255, 255, 255 ) );
		g.fillRect( x, 0, 1, height );
		g.fillRect( x + getLength() * 2, 0, 1, height );
	}

	public Thing findThing( int x, int y ){
		for ( Iterator it = this.objects.iterator(); it.hasNext(); ){
			Thing t = (Thing) it.next();
			// System.out.println( "Check " + t + " X1: " + t.getX1() + " Y1: " + t.getY1() + " X2: " + t.getX2() + " Y2: " + t.getY2() + " vs " + x + ", " + y );
			if ( x >= t.getX1() && x <= t.getX2() &&
			     y >= t.getY1() && y <= t.getY2() ){
				return t;
			}
		}
		return null;
	}

	public boolean hasThing( Thing t ){
		// System.out.println( this + " contains = " + this.objects.contains( t ) );
		for ( Iterator it = objects.iterator(); it.hasNext(); ){
			if ( it.next() == t ){
				return true;
			}
		}
		return false;
		// return this.objects.contains( t );
	}

	public int getLength(){
		return length;
	}

}
