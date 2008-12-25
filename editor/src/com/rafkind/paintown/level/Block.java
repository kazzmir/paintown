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
        private int id;
	private List objects;
	private boolean enabled = true;
	private boolean highlight = false;
	private boolean continuous = false;
	private int finish = 0;

	public Block( Token token ) throws LoadException {
		Token l = token.findToken( "length" );
		if ( l != null ){
			length = l.readInt( 0 );	
		}

                Token tid = token.findToken("id");
                if (tid != null){
                    id = tid.readInt(0);
                    Level.checkBlockId(id);
                } else {
                    id = Level.nextBlockId();
                }

		Token f = token.findToken( "finish" );
		if ( f != null ){
			finish = f.readInt( 0 );
		}

		Token b = token.findToken( "continuous" );
		if ( b != null ){
			continuous = true;
		}

		objects = new ArrayList();
		for ( Iterator it = token.findTokens( "object" ).iterator(); it.hasNext(); ){
			Token t = (Token) it.next();
			Token type = t.findToken( "type" );
			if ( type != null ){
				String str = type.readString( 0 );
				if ( str.equals( "enemy" ) ){
					objects.add( new Character( t ) );
				} else if ( str.equals( "item" ) ){
					objects.add( new Item( t ) );
				} else if ( str.equals( "cat" ) ){
					objects.add( new Cat( t ) );
				} else {
					System.out.println( "Warning: ignoring object of type '" + str + "' at line " + type.getLine() );
				}
			} else {
				throw new LoadException( "Object does not have a 'type' expression at line " + t.getLine() );
			}
		}
	}

	public Block(){
		this.objects = new ArrayList();
		this.length = 320;
	}

	public void setFinish( int s ){
		finish = s;
	}

	public boolean isFinish(){
		return finish > 0;
	}

	public boolean isContinuous(){
		return continuous;
	}

	public void setContinuous( boolean b ){
		this.continuous = b;
	}

	public int getFinish(){
		return finish;
	}

	public boolean isEnabled(){
		return enabled;
	}

	public void setEnabled( boolean b ){
		enabled = b;
	}

	public void addThing( Thing t ){
		objects.add( t );
	}

	public void removeThing( Thing t ){
		objects.remove( t );
	}

	public void removeAllThings(){
		objects.clear();
	}

	public void render( Graphics2D g, int x, int height, int minZ, int maxZ, int num ){
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
		g.translate( x, minZ );
		
		for ( int i = 0; i < objs.length; i++ ){
			Thing t = (Thing) objs[ i ];
			g.setColor( new Color( 255, 255, 255 ) );
			t.render( g, getHighlight() );
			g.drawString( "Block " + num, t.getX(), t.getY() );
		}
		g.translate( 0, -minZ );
		g.setColor( new Color( 255, 255, 255 ) );
		g.fillRect( 0, 0, 1, height );
		g.fillRect( getLength(), 0, 1, height );
		g.translate( -x, 0 );
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

	public Token toToken(){
		Token block = new Token();
		block.addToken( new Token( "block" ) );
		// block.addToken( new Token().addToken( new Token( "length" ) ).addToken( new Token( String.valueOf( getLength() ) ) ) );
		block.addToken(new String[]{"id", String.valueOf(getId())});
		block.addToken( new String[]{ "length", String.valueOf( getLength() ) } );
		if ( isContinuous() ){
			block.addToken( new String[]{ "continuous" } );
		}

		if ( isFinish() ){
			block.addToken( new String[]{ "finish", String.valueOf( getFinish() ) } );
		}
		for ( Iterator it = objects.iterator(); it.hasNext(); ){
			Thing t = (Thing) it.next();
			block.addToken( t.toToken() );
		}
		return block;
	}

	public List getThings(){
		return this.objects;
	}

	public void setHighlight( boolean h ){
		this.highlight = h;
	}

	public boolean getHighlight(){
		return this.highlight;
	}

	public void setLength( int i ){
		length = i;
	}

	public int getLength(){
		return length;
	}

        public int getId(){
            return id;
        }

        public void setId(int id){
            this.id = id;
        }
}
