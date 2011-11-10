package com.rafkind.paintown;

import java.util.*;

public class Token{

	private List tokens;
	private Token parent;
	private String data;
	private String currentData;
	private int line;

	public Token( Token parent ){
		this( 0, parent, null );
	}

	public Token( int line, Token parent ){
		this( line, parent, null );
	}

	public Token( int line ){
		this( line, (Token) null );
	}

	public Token(){
		this( (Token) null );
	}

	public Token( String data ){
		this( null, data );
	}

	public Token( Token parent, String data ){
		this( 0, parent, data );
	}

	public Token( int line, Token parent, String data ){
		tokens = new ArrayList();
		this.parent = parent;
		this.data = data;
		this.line = line;
		currentData = "";
	}

	public int getLine(){
		return line;
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
			String stripped = currentData;
			if ( stripped.matches( "^\".*\"$" ) ){
				stripped = stripped.substring( 1, stripped.length() - 1 );
			}
			// this.tokens.add( new Token( this, currentData ) );
			this.tokens.add( new Token( this, stripped ) );
			currentData = "";
		}
	}

	public void addData( char c ){
		currentData += String.valueOf( c );
	}

	public String getName(){
		if ( tokens.isEmpty() ){
			return "no name";
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

	public List<Token> findTokens( String name ){
		int seperator = name.indexOf( "/" );
		String part = null;
		if ( seperator != -1 ){
			part = name.substring( 0, seperator );
			name = name.substring( seperator + 1 );
		}
		List<Token> all = new ArrayList();
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

    public boolean readBoolean(int index){
        return Boolean.parseBoolean(readIndex(index).toString());
    }

	public double readDouble( int index ){
		return Double.parseDouble( readIndex( index ).toString() );
	}

	public String readString( int index ){
		return readIndex( index ).toString();
	}

	public Token addToken( Token n ){
		tokens.add( n );
		return this;
	}

	public Token addToken( String[] args ){
		Token t = new Token( this );
		for ( int i = 0; i < args.length; i++ ){
			t.addToken( new Token( args[ i ] ) );
		}
		return addToken( t );
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
			b.append( t.toString( spaces + 2 ) );
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
