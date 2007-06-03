package com.rafkind.paintown.level;

import java.awt.*;
import java.awt.image.*;
import java.io.*;
import javax.imageio.*;

import java.util.Iterator;
import java.util.List;
import java.util.ArrayList;
import java.util.HashMap;
import com.rafkind.paintown.exception.LoadException;
import com.rafkind.paintown.TokenReader;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.MaskedImage;

public class Level{

	private String name;
	private Image background;
	private int width;
	private List frontPanels;
	private HashMap backPanels;
	private List panelOrder;
	private int minZ;
	private int maxZ;

	private List blocks;

	public Level(){
		initAll();
	}

	public List getBlocks(){
		return blocks;
	}

	public int getMinZ(){
		return minZ;
	}

	private void drawFrontPanels( Graphics2D g ){
		int w = 0;
		if ( ! frontPanels.isEmpty() ){
			while ( w < getWidth() ){
				for ( Iterator it = frontPanels.iterator(); it.hasNext(); ){
					Image panel = (Image) it.next();
					g.drawImage( panel, w, 0, null );
					w += panel.getWidth( null );
				}
			}
		}
	}

	private void drawBackground( Graphics2D g ){
		if ( background != null ){
			int w = 0;
			while ( w < getWidth() ){
				g.drawImage( background, w, 0, null );
				w += background.getWidth( null );
			}
		}
	}

	private void drawBackPanels( Graphics2D g ){
		int w = 0;
		for ( Iterator it = this.panelOrder.iterator(); it.hasNext(); ){
			Integer i = (Integer) it.next();
			Image image = (Image) this.backPanels.get( i );
			g.drawImage( image, w, 0, null );
			w += image.getWidth( null );
		}
	}

	private void drawBlocks( Graphics2D g, int height ){
		int w = 0;
		for ( Iterator it = this.blocks.iterator(); it.hasNext(); ){
			Block b = (Block) it.next();
			if ( b.isEnabled() ){
				b.render( g, w, height, minZ, maxZ );
				w += b.getLength();
			}
		}
	}

	public void render( Graphics2D g, int x, int y, int width, int height ){
		g.clearRect( 0, 0, getWidth(), getHeight() );
		g.scale( 2, 2 );
		drawBackground( g );
		drawBackPanels( g );
		drawBlocks( g, height );
		drawFrontPanels( g );	
	}

	public Thing findThing( int x, int y ){
		for ( Iterator it = blocks.iterator(); it.hasNext(); ){
			Block b = (Block) it.next();
			if ( b.isEnabled() ){
				Thing t = b.findThing( x, y - getMinZ() );
				if ( t != null ){
					return t;
				}
				x -= b.getLength();
			}
		}
		return null;
	}

	public void moveThing( Thing thing, int x, int y ){
		for ( Iterator it = blocks.iterator(); it.hasNext(); ){
			Block b = (Block) it.next();
			if ( b.isEnabled() ){
				if ( b.hasThing( thing ) ){
					System.out.println( b + ". Move " + thing + " to " + x + " " + (y - getMinZ()) );
					thing.setX( x );
					thing.setY( y - getMinZ() );
				} else {
					System.out.println( b + " does not have " + thing );
				}
				x -= b.getLength();
			}
		}
	}

	private void initAll(){
		this.name = null;
		this.background = null;
		this.minZ = 0;
		this.maxZ = 0;

		this.width = 640;
		this.frontPanels = new ArrayList();
		this.backPanels = new HashMap();
		this.panelOrder = new ArrayList();
		this.blocks = new ArrayList();
	}

	public void load( File f ) throws LoadException {
		initAll();
		TokenReader reader = new TokenReader( f );
		Token head = reader.nextToken();
		if ( ! head.getName().equals( "level" ) ){
			throw new LoadException( "Starting token is not 'level'" );
		}
		
		Token z = head.findToken( "z" );
		if ( z != null ){
			Token min = z.findToken( "minimum" );
			if ( min != null ){
				minZ = min.readInt( 0 );
			}
			Token max = z.findToken( "maximum" );
			if ( max != null ){
				maxZ = max.readInt( 0 );
			}
		}

		loadBackground( head.findToken( "background" ) );

		// setWidth( calculateWidth( head.findTokens( "block/length" ) ) );
		for ( Iterator it = head.findTokens( "frontpanel" ).iterator(); it.hasNext(); ){
			Token t = (Token) it.next();
			String file = t.readString( 0 );
			frontPanels.add( loadImage( file ) );
		}

		for ( Iterator it = head.findTokens( "panel" ).iterator(); it.hasNext(); ){
			Token t = (Token) it.next();
			int index = t.readInt( 0 );
			String file = t.readString( 1 );
			this.backPanels.put( new Integer( index ), loadImage( file  ) );
		}

		Token order = head.findToken( "order" );
		if ( order != null ){
			for ( Iterator it = order.iterator(); it.hasNext(); ){
				panelOrder.add( Integer.valueOf( it.next().toString() ) );	
			}
		} else {
			System.out.println( "No 'order' token given" );
		}

		for ( Iterator it = head.findTokens( "block" ).iterator(); it.hasNext(); ){
			Token t = (Token) it.next();
			this.blocks.add( new Block( t ) );
		}


		System.out.println( "Loaded " + f );
	}

	private Image loadImage( String s ) throws LoadException {
		try{
			return MaskedImage.load( s );
			/*
			BufferedImage temp = ImageIO.read( new File( s ) );
			BufferedImage image = new BufferedImage( temp.getWidth(), temp.getHeight(), BufferedImage.TYPE_INT_ARGB );
			for ( int x = 0; x < temp.getWidth(); x++ ){
				for ( int y = 0; y < temp.getHeight(); y++ ){
					int pixel = temp.getRGB( x, y );
					if ( (pixel & 0x00ffffff) == 0x00ff00ff ){
						/ * convert masking color into an alpha channel that is translucent * /
						pixel = 0x00ffffff;
					}
					image.setRGB( x, y, pixel );
				}
			}
			return image;
			*/
		} catch ( IOException ie ){
			throw new LoadException( "Could not load " + s );
		}
	}

	private void loadBackground( Token t ) throws LoadException {
		if ( t != null ){
			String s = String.valueOf( t.iterator().next() );
			setBackground( loadImage( s ) );
		}
	}

	private void setWidth( int w ){
		width = w;
	}

	private int calculateWidth( List lengths ){
		int w = 0;
		for ( Iterator it = lengths.iterator(); it.hasNext(); ){
			Token t = (Token) it.next();
			w += t.readInt( 0 ) * 2;
		}
		return w;
	}
	
	private void setBackground( Image i ){
		background = i;
	}

	public Dimension getSize(){
		return new Dimension( getWidth(), getHeight() );
	}

	private int getWidth(){
		int w = 0;
		for ( Iterator it = blocks.iterator(); it.hasNext(); ){
			Block b = (Block) it.next();
			if ( b.isEnabled() ){
				w += b.getLength() * 2;
			}
		}
		return w;
	}

	private int getHeight(){
		return 480;
	}
}
