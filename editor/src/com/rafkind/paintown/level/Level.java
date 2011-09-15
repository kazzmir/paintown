package com.rafkind.paintown.level;

import java.awt.*;
import java.awt.image.*;
import java.io.*;
import javax.imageio.*;

import java.util.Iterator;
import java.util.List;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import com.rafkind.paintown.exception.LoadException;
import com.rafkind.paintown.TokenReader;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.MaskedImage;
import com.rafkind.paintown.Editor;

public class Level{

    private String name;
    private Image background;
    private String backgroundFile;
    private int width;
    private List frontPanels;
    private HashMap backPanels;
    private List panelOrder;
    private int minZ;
    private int maxZ;
    private double backgroundParallax;
    private double foregroundParallax;
    private double scale;
    private File path;
    private String atmosphere;
    private String description;
    private Token trigger;

    private List blocks;

    private static int nextId = 0;
    private static int nextBlockId = 0;

    public static int nextId(){
        nextId += 1;
        return nextId;
    }

    public static void checkId(int i){
        if (i > nextId){
            nextId = i;
        }
    }

    public static void resetId(){
        nextId = 0;
    }

    public static int nextBlockId(){
        nextBlockId += 1;
        return nextBlockId;
    }

    public static void checkBlockId(int i){
        if (i > nextBlockId){
            nextBlockId = i;
        }
    }

    public static void resetBlockId(){
        nextBlockId = 0;
    }

    private class Panel{
        public String name;
        public Image image;
        public Panel( String name, Image i ){
            this.name = name;
            this.image = i;
        }
    }

    public Level(){
        initAll();
        this.path = null;
    }

    public File getPath(){
        return path;
    }

    public void setPath( File path ){
        this.path = path;
    }

    public Level( File path ) throws LoadException {
        this.path = path;
        load( path );
    }

    public void setAtmosphere( String s ){
        this.atmosphere = s;
    }

    public String getAtmosphere(){
        return this.atmosphere;
    }

    public void setBackgroundParallax( double d ){
        this.backgroundParallax = d;
    }

    public double getBackgroundParallax(){
        return this.backgroundParallax;
    }

    public void setForegroundParallax( double d ){
        this.foregroundParallax = d;
    }

    public double getForegroundParallax(){
        return this.foregroundParallax;
    }

    public String getDescription(){
        return this.description;
    }

    public void setDescription(String description){
        this.description = description;
    }

    public List getBlocks(){
        return blocks;
    }

    public int getMinZ(){
        return minZ;
    }

    public int getMaxZ(){
        return maxZ;
    }

    public void setMinZ( int z ){
        minZ = z;
    }

    public void setMaxZ( int z ){
        maxZ = z;
    }

    public void addFrontPanel( String s ) throws LoadException {
        frontPanels.add( new Panel( s, loadImage( s ) ) );
    }

    public List getBackPanelOrder(){
        return new ArrayList( panelOrder );
    }

    public void addBackPanelOrder( String path ){
        for ( Iterator it = this.backPanels.entrySet().iterator(); it.hasNext(); ){
            Map.Entry entry = (Map.Entry) it.next();
            Integer key = (Integer) entry.getKey();
            Panel panel = (Panel) entry.getValue();
            if ( panel.name.equals( path ) ){
                panelOrder.add( key );
                return;
            }
        }
    }

    public void removeLastOrder(){
        if ( ! panelOrder.isEmpty() ){
            panelOrder.remove( panelOrder.size() - 1 );
        }
    }

    private Integer findFreeKey( HashMap map ){
        int i = 0;
        for ( Iterator it = map.keySet().iterator(); it.hasNext(); ){
            Integer x = (Integer) it.next();
            if ( i <= x.intValue() ){
                i = x.intValue() + 1;	
            }
        }
        return new Integer( i );
    }

    public void addBackPanel( String path ) throws LoadException {
        this.backPanels.put( findFreeKey( backPanels ), new Panel( path, loadImage( path ) ) );
    }

    public String getBackPanelName( int i ){
        Panel panel = (Panel) backPanels.get( new Integer( i ) );
        return panel.name;
    }

    public void removeBackPanel( String path ){
        for ( Iterator it = this.backPanels.entrySet().iterator(); it.hasNext(); ){
            Map.Entry entry = (Map.Entry) it.next();
            Integer key = (Integer) entry.getKey();
            Panel panel = (Panel) entry.getValue();
            if ( panel.name.equals( path ) ){
                this.backPanels.remove( key );
                for ( Iterator orders = this.panelOrder.iterator(); orders.hasNext(); ){
                    Integer o = (Integer) orders.next();
                    if ( o.equals( key ) ){
                        orders.remove();	
                    }
                }
                return;
            }
        }
    }

    public List getFrontPanelNames(){
        List names = new ArrayList();
        for ( Iterator it = frontPanels.iterator(); it.hasNext(); ){
            Panel panel = (Panel) it.next();
            names.add( panel.name );
        }
        return names;
    }

    public List getBackPanelNames(){
        List names = new ArrayList();
        for ( Iterator it = backPanels.values().iterator(); it.hasNext(); ){
            Panel panel = (Panel) it.next();
            names.add( panel.name );
        }
        return names;
    }

    public void removeFrontPanel( String s ){
        for ( Iterator it = frontPanels.iterator(); it.hasNext(); ){
            Panel panel = (Panel) it.next();
            if ( panel.name.equals( s ) ){
                frontPanels.remove( panel );
                break;
            }
        }
    }

    private void drawFrontPanels( Graphics2D g ){
        int w = 0;
        if ( ! frontPanels.isEmpty() ){
            while ( w < getWidth() / getScale() ){
                int ow = w;
                for ( Iterator it = frontPanels.iterator(); it.hasNext(); ){
                    Panel p = (Panel) it.next();
                    Image panel = (Image) p.image;
                    g.drawImage( panel, w, 0, null );
                    w += panel.getWidth( null );
                }
                /* make sure not to get into an infinite loop due to lack
                 * of width on the images
                 */
                if ( w == ow ){
                    w += 1;
                }
            }
        }
    }

    private void drawBackground( Graphics2D g ){
        if ( background != null ){
            int w = 0;
            while ( w < getWidth() / getScale() ){
                g.drawImage( background, w, 0, null );
                w += background.getWidth( null );
            }
        }
    }

    private void drawBackPanels( Graphics2D g ){
        int w = 0;
        for ( Iterator it = this.panelOrder.iterator(); it.hasNext(); ){
            Integer i = (Integer) it.next();
            Image image = ((Panel) this.backPanels.get( i )).image;
            g.drawImage( image, w, 0, null );
            w += image.getWidth( null );
        }
    }

    private void drawBlocks( Graphics2D g, int height ){
        int w = 0;
        int num = 1;
        for ( Iterator it = this.blocks.iterator(); it.hasNext(); ){
            Block b = (Block) it.next();
            if ( b.isEnabled() ){
                b.render( g, w, height, minZ, maxZ, num );
                g.drawString( "Block " + num, w + 5, 15 );
                w += b.getLength();
            }
            num += 1;
        }
    }

    public void render( Graphics2D g, int x, int y, int width, int height ){
        // g.clearRect( 0, 0, (int) getWidth(), (int) getHeight() );
        g.scale( getScale(), getScale() );
        drawBackground( g );
        drawBackPanels( g );
        g.setColor( new Color( 255, 0, 0 ) );
        g.drawLine( 0, getMinZ(), (int)(getWidth() / getScale()), getMinZ() );
        g.drawLine( 0, getMaxZ(), (int)(getWidth() / getScale()), getMaxZ() );
        drawBlocks( g, height );
        drawFrontPanels( g );	
    }

    public Block findBlock( Thing t ){
        for ( Iterator it = blocks.iterator(); it.hasNext(); ){
            Block b = (Block) it.next();
            if ( b.hasThing( t ) ){
                return b;
            }
        }
        return null;
    }

    /* find the block that contains point x */
    public Block findBlock( int x ){
        int total = 0;
        for ( Iterator it = getBlocks().iterator(); it.hasNext(); ){
            Block b = (Block) it.next();
            if ( b.isEnabled() ){
                if ( x >= total && x <= total + b.getLength() ){
                    return b;
                }
                total += b.getLength();
            }
        }
        return null;
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
                    int my = y - getMinZ();
                    if ( my > getMaxZ() - getMinZ() ){
                        my = getMaxZ() - getMinZ();
                    }
                    if ( my < 0 ){
                        my = 0;
                    }
                    // System.out.println( b + ". Move " + thing + " to " + x + " " + my );
                    thing.setX( x );
                    thing.setY( my );
                } else {
                    // System.out.println( b + " does not have " + thing );
                }
                // x -= b.getLength();
            }
        }
    }

    public void initAll(){
        this.name = null;
        this.background = null;
        this.backgroundFile = null;
        this.minZ = 100;
        this.maxZ = 200;
        this.scale = 2;
        this.backgroundParallax = 5;
        this.foregroundParallax = 1.2;
        this.atmosphere = null;

        this.width = 640;
        this.frontPanels = new ArrayList();
        this.backPanels = new HashMap();
        this.panelOrder = new ArrayList();
        this.blocks = new ArrayList();

        resetId();
        resetBlockId();
    }

    private void load( File f ) throws LoadException {
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

        Token desc = head.findToken("description");
        if (desc != null){
            setDescription(desc.readString(0));
        }

        trigger = head.findToken("trigger");

        Token atm = head.findToken( "atmosphere" );
        if ( atm != null ){
            setAtmosphere( atm.readString( 0 ) );
        }

        Token parallax = head.findToken( "background-parallax" );
        if ( parallax != null ){
            setBackgroundParallax( parallax.readDouble( 0 ) );
        }
        parallax = head.findToken( "foreground-parallax" );
        if ( parallax != null ){
            setForegroundParallax( parallax.readDouble( 0 ) );
        }

        loadBackground( head.findToken( "background" ) );

        // setWidth( calculateWidth( head.findTokens( "block/length" ) ) );
        for ( Iterator it = head.findTokens( "frontpanel" ).iterator(); it.hasNext(); ){
            Token t = (Token) it.next();
            String file = t.readString( 0 );
            frontPanels.add( new Panel( file, loadImage( file ) ) );
        }

        for ( Iterator it = head.findTokens( "panel" ).iterator(); it.hasNext(); ){
            Token t = (Token) it.next();
            int index = t.readInt( 0 );
            String file = t.readString( 1 );
            this.backPanels.put( new Integer( index ), new Panel( file, loadImage( file ) ) );
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
            return MaskedImage.load( Editor.dataPath( s ) );
        } catch ( IOException ie ){
            throw new LoadException( "Could not load " + s );
        }
    }

    private void loadBackground( Token t ) throws LoadException {
        if ( t != null ){
            String s = String.valueOf( t.iterator().next() );
            setBackground( loadImage( s ) );
            backgroundFile = s;
        }
    }

    public void loadBackground( String s ){
        try{
            setBackground( loadImage( s ) );
            backgroundFile = s;
        } catch ( LoadException e ){
            e.printStackTrace();
        }
    }

    public String getBackgroundFile(){
        return backgroundFile;
    }

    private void setWidth( int w ){
        width = w;
    }

    private int calculateWidth( List lengths ){
        int w = 0;
        for ( Iterator it = lengths.iterator(); it.hasNext(); ){
            Token t = (Token) it.next();
            w += t.readInt( 0 ) * getScale();
        }
        return w;
    }

    private void setBackground( Image i ){
        background = i;
    }

    public Dimension getSize(){
        return new Dimension( (int) getWidth(), (int) getHeight() );
    }

    public double getWidth(){
        double w = 0;
        for ( Iterator it = blocks.iterator(); it.hasNext(); ){
            Block b = (Block) it.next();
            if ( b.isEnabled() ){
                w += b.getLength() * getScale();
            }
        }
        return w;
    }

    public Token toToken(){
        Token level = new Token();
        level.addToken( new Token( level, "level" ) );
        Token z = new Token( level );
        level.addToken( z );
        z.addToken( new Token( z, "z" ) );
        z.addToken( new String[]{ "minimum", String.valueOf( getMinZ() ) } );
        z.addToken( new String[]{ "maximum", String.valueOf( getMaxZ() ) } );
        if ( getAtmosphere() != null ){
            level.addToken( new String[]{ "atmosphere", getAtmosphere() } );
        }
        level.addToken( new String[]{ "background-parallax", String.valueOf( getBackgroundParallax() ) } );
        level.addToken( new String[]{ "foreground-parallax", String.valueOf( getForegroundParallax() ) } );

        if ( backgroundFile != null ){
            level.addToken( new String[]{ "background", "\"" + backgroundFile.replaceAll( "\\\\", "/" ) + "\"" } );
        }

        for ( Iterator it = frontPanels.iterator(); it.hasNext(); ){
            /*
               Token f = new Token( level );
               level.addToken( f );
               Panel p = (Panel) it.next();
               f.addToken( new Token( f, "frontpanel" ) );
               f.addToken( new Token( f, p.name ) );
               */
            Panel p = (Panel) it.next();
            level.addToken( new String[]{ "frontpanel", "\"" + p.name.replaceAll( "\\\\", "/" ) + "\"" } );
        }

        for ( Iterator it = backPanels.entrySet().iterator(); it.hasNext(); ){
            Map.Entry entry = (Map.Entry) it.next();
            Token f = new Token( level );
            level.addToken( f );
            f.addToken( new Token( f, "panel" ) );
            Panel p = (Panel) entry.getValue();
            f.addToken( new Token( f, entry.getKey().toString() ) );
            f.addToken( new Token( f, "\"" + p.name.replaceAll( "\\\\", "/" ) + "\"" ) );
        }

        if (getDescription() != null){
            level.addToken(new String[]{ "description", "\"" + getDescription() + "\""});
        }

        if (trigger != null){
            level.addToken(trigger);
        }

        Token order = new Token( level );
        level.addToken( order );
        order.addToken( new Token( order, "order" ) );
        for ( Iterator it = panelOrder.iterator(); it.hasNext(); ){
            order.addToken( new Token( order, it.next().toString() ) );
        }

        for ( Iterator it = blocks.iterator(); it.hasNext(); ){
            Block b = (Block) it.next();
            level.addToken( b.toToken() );
        }

        return level;
    }

    public double getScale(){
        return scale;
    }

    public void setScale( double s ){
        scale = s;
    }

    public double getHeight(){
        return 240 * getScale();
    }
}
