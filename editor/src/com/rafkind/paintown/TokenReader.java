package com.rafkind.paintown;

import java.util.*;
import java.io.*;
import com.rafkind.paintown.exception.LoadException;

public class TokenReader{

	private List tokens;
	private Iterator iterator;

	public TokenReader( File f ) throws LoadException {
		try{
			tokens = new ArrayList();
			BufferedReader reader = new BufferedReader(new FileReader(f));
	
			Token current = null;
			int line = 1;
			boolean inString = false;
			while ( reader.ready() ){
				int i = reader.read();
				if ( i == -1 ){
					break;
				}
				char c = (char) i;

				if ( inString ){
					if ( c == '"' ){
						inString = false;
						current.addData( '"' );
					} else {
						if ( current != null ){
							current.addData( c );
						}
					}
					continue;
				}

				switch ( c ){
					case '(' : {
						// System.out.println( "Read new token" );
						if ( current == null ){
							current = new Token( line );
							tokens.add( current );
						} else {
							current.endData();
							Token n = new Token( line, current );
							current.addToken( n );
							current = n;
						}
						break;
					}
					case '"' : {
						inString = true;
						current.addData( '"' );
						break;
					}
					case '	' :
					case ' ' : {
						/* whitespace */
						if ( current != null ){
							current.endData();
						}
						break;
					}
					case '#' :
					case ';' : {
						reader.readLine();
						line += 1;
						break;
					}
					case ')' : {
						if ( current == null ){
							throw new LoadException( "Too many ')' found. Line " + line + ". File " + f.getName() );
						} else {
							current.endData();
							current = current.getParent();
						}
						break;
					}
					case '\n' : {
						line += 1;
						if ( current != null ){
							current.endData();
						}
						break;
					}
					case 'a' :
					case 'b' :
					case 'c' :
					case 'd' :
					case 'e' :
					case 'f' :
					case 'g' :
					case 'h' :
					case 'i' :
					case 'j' :
					case 'k' :
					case 'l' :
					case 'm' :
					case 'n' :
					case 'o' :
					case 'p' :
					case 'q' :
					case 'r' :
					case 's' :
					case 't' :
					case 'u' :
					case 'v' :
					case 'w' :
					case 'x' :
					case 'y' :
					case 'z' :

					case 'A' :
					case 'B' :
					case 'C' :
					case 'D' :
					case 'E' :
					case 'F' :
					case 'G' :
					case 'H' :
					case 'I' :
					case 'J' :
					case 'K' :
					case 'L' :
					case 'M' :
					case 'N' :
					case 'O' :
					case 'P' :
					case 'Q' :
					case 'R' :
					case 'S' :
					case 'T' :
					case 'U' :
					case 'V' :
					case 'W' :
					case 'X' :
					case 'Y' :
					case 'Z' :

					case '/' :
					case '\\' :
					case '\'' :
					case '.' :
					case ':' :
					case '!' :
					case '@' :
					case '$' :
					case '%' :
					case '^' :
					case '&' :
					case '_' :
					case '-' :
					case '0' :
					case '1' :
					case '2' :
					case '3' :
					case '4' :
					case '5' :
					case '6' :
					case '7' :
					case '8' :
					case '9' : {
						if ( current != null ){
							current.addData( c );
						}
						break;
					}
				}
			}

			System.out.println( "Read " + (line - 1) + " lines" );

			/*
			Token f1 = (Token) tokens.get( 0 );
			System.out.println( f1 );
			System.out.println( f1.findToken( "background" ) );
			*/

            reader.close();

		} catch ( IOException ie ){
			throw new LoadException( "Could not read file", ie );
		}

		filterTokens( tokens, "!" );

		reset();
	}

	public void reset(){
		iterator = tokens.iterator();
	}

	/* remove any tokens that start with the invalid string */
	private void filterTokens( List tokens, String invalid ){
		List filtered = new ArrayList();
		for ( Iterator it = tokens.iterator(); it.hasNext(); ){
			Token t = (Token) it.next();
			if ( ! t.isDatum() && t.getName().startsWith( invalid ) ){
				filtered.add( t );
			} else if ( ! t.isDatum() ){
				filterTokens( t.getTokens(), invalid );
			}
		}
		for ( Iterator it = filtered.iterator(); it.hasNext(); ){
			tokens.remove( it.next() );
		}
	}

	public Token nextToken() throws LoadException {
		if ( ! iterator.hasNext() ){
			throw new LoadException( "No tokens left" );
		}
		return (Token) iterator.next();
	}
}
