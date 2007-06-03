package com.rafkind.paintown;

import java.util.*;

public class Token{

	private List tokens;
	private Token parent;
	private String data;
	private String currentData;

	public Token( Token parent ){
		this( parent, null );
	}

	public Token( Token parent, String data ){
		tokens = new ArrayList();
		this.parent = parent;
		this.data = data;
		currentData = "";
	}

	public List getTokens(){
		return tokens;
	}

	public Iterator iterator(){
		Iterator i = tokens.iterator();
		if ( i.hasNext() ){
			i.next();
		}
		return i;
	}

	public boolean isDatum(){
		return this.tokens.isEmpty();
	}

	public void endData(){
		if ( ! currentData.equals( "" ) ){
			// System.out.println( "Added " + currentData );
			this.tokens.add( new Token( this, currentData ) );
			currentData = "";
		}
	}

	public void addData( char c ){
		currentData += String.valueOf( c );
	}

	public String getName(){
		if ( tokens.isEmpty() ){
			return ":(";
		}
		return ((Token) tokens.get( 0 )).getData();
	}

	private String getData(){
		return data;
	}

	public Token findToken( String name ){
		/*
		for ( Iterator it = this.iterator(); it.hasNext(); ){
			Token t = (Token) it.next();
			if ( t.getName().equals( name ) ){
				return t;
			}
		}
		return null;
		*/
		List l = findTokens( name );
		if ( l.isEmpty() ){
			return null;
		}
		return (Token) l.get( 0 );
	}

	public List findTokens( String name ){
		int seperator = name.indexOf( "/" );
		String part = null;
		if ( seperator != -1 ){
			part = name.substring( 0, seperator );
			name = name.substring( seperator + 1 );
		}
		List all = new ArrayList();
		for ( Iterator it = this.iterator(); it.hasNext(); ){
			Token t = (Token) it.next();
			if ( part == null ){
				if ( t.getName().equals( name ) ){
					all.add( t );
				}
			} else {
				if ( t.getName().equals( part ) ){
					all.addAll( t.findTokens( name ) );
				}
			}
		}
		return all;
	}

	private Token readIndex( int index ){
		Iterator it = this.iterator();
		for ( int i = 0; i < index; i++ ){
			it.next();
		}
		return (Token) it.next();
	}

	public int readInt( int index ){
		return Integer.parseInt( readIndex( index ).toString() );
	}

	public String readString( int index ){
		return readIndex( index ).toString();
	}

	public void addToken( Token n ){
		tokens.add( n );
	}

	public void removeToken( Token n ){
		tokens.remove( n );
	}

	public String toString( int spaces ){
		if ( isDatum() ){
			return getData();
		}
		StringBuffer b = new StringBuffer();
		b.append( "\n" );
		for ( int i = 0; i < spaces; i++ ){
			b.append( " " );
		}
		b.append( "(" + getName() );	
		for ( Iterator it = this.iterator(); it.hasNext(); ){
			Token t = (Token) it.next();
			b.append( " " );
			b.append( t.toString( spaces + 3 ) );
		}
		b.append( ")" );
		return b.toString();
	}

	public String toString(){
		return toString( 0 );
	}
	
	public Token getParent(){
		return this.parent;
	}
}
