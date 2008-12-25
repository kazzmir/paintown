package com.rafkind.paintown.level;

import java.awt.*;
import java.io.*;
import java.awt.image.*;

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
import com.rafkind.paintown.Editor;

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

		Token health = token.findToken( "health" );
		if ( health != null ){
			setHealth( health.readInt( 0 ) );
		} else {
			throw new LoadException( "No health token given" );
		}

		Token data = new TokenReader( new File( Editor.dataPath( getPath() ) ) ).nextToken();
		maxMaps = calculateMaxMaps( data );

		remaps = new HashMap();

		this.remaps.put( new Integer( 0 ), new Lambda0(){
			public Object invoke(){
				return getMain();
			}
		});

		int num = 1;
		for ( Iterator it = data.findTokens( "remap" ).iterator(); it.hasNext(); ){
			Token re = (Token) it.next();
			final String normalPath = re.readString( 0 );
			final String altPath = re.readString( 1 );
			final int tnum = num;
			synchronized( remaps ){
				this.remaps.put( new Integer( tnum ), new Lambda0(){
					public Object invoke(){
						try{
							final Image alt = createRemap( Editor.dataPath( normalPath ), Editor.dataPath( altPath ), getMain() );
							synchronized( remaps ){
								remaps.put( new Integer( tnum ), new Lambda0(){
									public Object invoke(){
										return alt;
									}
								});
							}
							return alt;
						} catch ( IOException ie ){
							ie.printStackTrace();
							synchronized( remaps ){
								remaps.put( new Integer( tnum ), new Lambda0(){
									public Object invoke(){
										return getMain();
									}
								});
							}
							return getMain();
						}
					}
				});
			}
			num += 1;
		}
	}

	public Character( Character copy ){
		super( copy );
		setRemaps( copy.getRemaps() );
		setMap( copy.getMap() );
		setAggression( copy.getAggression() );
		setMaxMaps( copy.getMaxMaps() );
		setHealth( copy.getHealth() );
	}

	private HashMap getRemaps(){
		return remaps;
	}

	private void setRemaps( HashMap remaps ){
		this.remaps = remaps;
	}

	private Image createRemap( String normal, String alt, BufferedImage original ) throws IOException {
		HashMap colors = new HashMap();
		BufferedImage old = MaskedImage.load( normal );
		BufferedImage xnew = MaskedImage.load( alt );
		BufferedImage map = new MaskedImage( original.getWidth( null ), original.getHeight( null ) );

		for ( int x = 0; x < old.getWidth( null ); x += 1 ){
			for ( int y = 0; y < old.getHeight( null ); y += 1 ){
				int oldPixel = old.getRGB( x, y );
				int newPixel = xnew.getRGB( x, y );
				if ( oldPixel != newPixel ){
					colors.put( new Integer( oldPixel ), new Integer( newPixel ) );
				}
			}
		}

		for ( int x = 0; x < original.getWidth( null ); x += 1 ){
			for ( int y = 0; y < original.getHeight( null ); y += 1 ){
				int pixel = original.getRGB( x, y );
				Integer r = (Integer) colors.get( new Integer( pixel ) );
				if ( r != null ){
					pixel = r.intValue();
				}
				map.setRGB( x, y, pixel );
			}
		}

		return map;
	}
	
	public Thing copy(){
		return new Character( this );
	}

	public void render( Graphics2D g, boolean highlight ){
		Lambda0 proc = null;
		synchronized( remaps ){
			proc = (Lambda0) this.remaps.get( new Integer( getMap() ) );
			if ( proc == null ){
				proc = (Lambda0) this.remaps.get( new Integer( 0 ) );
			}
		}
		Image i = (Image) proc.invoke_();
		render( i, g, highlight );
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

	private void setMaxMaps( int i ){
		maxMaps = i;
	}

	private int calculateMaxMaps( Token head ) throws LoadException {
		return head.findTokens( "remap" ).size();
	}

	protected BufferedImage readIdleImage( String file ) throws LoadException {
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
					return MaskedImage.load( Editor.dataPath( base + pic ) );
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
	
	public String toString(){
		return super.toString() + " Health: " + getHealth();
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
		thing.addToken( new String[]{"id", String.valueOf(getId())} );
		thing.addToken( new String[]{ "name", "\"" + getName() + "\"" } );
		thing.addToken( new String[]{ "path", "\"" + getPath() + "\"" } );
		thing.addToken( new String[]{ "map", String.valueOf( getMap() ) } );
		thing.addToken( new String[]{ "health", String.valueOf( getHealth() ) } );
		if ( getAggression() != -1 ){
			thing.addToken( new String[]{ "aggression", String.valueOf( getAggression() ) } );
		}
		thing.addToken( new String[]{ "coords", String.valueOf( getX() ), String.valueOf( getY() ) } );
		return thing;
	}

}
