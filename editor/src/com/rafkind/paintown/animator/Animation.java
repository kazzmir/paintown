package com.rafkind.paintown.animator;

import java.util.List;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Vector;

import com.rafkind.paintown.animator.events.*;

import com.rafkind.paintown.Token;
import com.rafkind.paintown.exception.*;

import java.awt.image.*;

import java.awt.Graphics;
import javax.swing.JComponent;

public class Animation implements Runnable {
	
	private String name;
	private boolean alive = true;
	private boolean running;
	private List drawables;
	private Vector events;
	private BufferedImage image;
	private BoundingBox attackArea;
	private int eventIndex;
	private int delay;
	private int delayTime;
	private int x;
	private int y;
	private int offsetX;
	private int offsetY;
	private String baseDirectory;
	private int range;
	private String type;
	
	private Vector keys;

	public Animation(){
		drawables = new ArrayList();
		events = new Vector();
		/* give the animation something so it rests a little bit */
		events.add( new NopEvent() );
		image = null;
		attackArea = new BoundingBox( 0, 0, 0, 0 );
		keys = new Vector();
		baseDirectory = "";
		type = "none";
	}

	public Animation( Vector events ){
		this();
		this.events = events;
	}

	public void setRange( int r ){
		range = r;
	}

	public int getRange(){
		return range;
	}

	public void addKey( String key ){
		keys.add( key );
	}

	public void removeKey( int index ){
		keys.remove( index );
	}

	public Vector getKeys(){
		return keys;
	}

	public String toString(){
		return name != null ? name : "No name set";
	}

	public void setAttack( BoundingBox attack ){
		attackArea = attack;
	}

	private BufferedImage currentImage(){
		return image;
	}

	public void setName( String s ){
		this.name = s;
	}

	public String getName(){
		return name;
	}

	public synchronized void setImage( BufferedImage image ){
		this.image = image;
		updateDrawables();
	}

	/* tell the components that know about this animation that its time
	 * to redraw the canvas.
	 * for things like updating the current frame or the attack box
	 */
	private void updateDrawables(){
		synchronized( drawables ){
			for ( Iterator it = drawables.iterator(); it.hasNext(); ){
				JComponent draw = (JComponent) it.next();	
				draw.repaint();
			}
		}
	}

	public synchronized void kill(){
		alive = false;
	}

	private synchronized boolean isAlive(){
		return alive;
	}

	private synchronized boolean isRunning(){
		return running;
	}

	public synchronized void stopRunning(){
		running = false;
	}
	
	public synchronized void startRunning(){
		running = true;
	}

	public void addEvent( AnimationEvent e ){
		synchronized( events ){
			events.add( e );
		}
	}

	public void clearEvents(){
		synchronized( events ){
			events.clear();
		}
	}

	/* swap position of e1 and e2 within event structure */
	public void swapEvents( int e1, int e2 ){
		synchronized( events ){
			Object a1 = events.get( e1 );
			Object a2 = events.get( e2 );
			events.set( e1, a2 );
			events.set( e2, a1 );
		}
		
	}

	/* return a new list that contains the current events
	 * do not edit the returned list, to add new events use
	 * addEvent/removeEvent
	 */ 
	public Vector getEvents(){
		synchronized( events ){
			return new Vector( events );
		}
	}

	public void removeEvent( AnimationEvent e ){
		synchronized( events ){
			events.remove( e );
		}
	}

	public void removeEvent( int i ){
		synchronized( events ){
			events.remove( i );
		}
	}

	public synchronized void draw( Graphics g, int x, int y ){
		int trueX = x + this.x + this.offsetX;
		int trueY = y + this.y + this.offsetY;
		if ( currentImage() != null ){
			g.drawImage( currentImage(), trueX, trueY, null );
		}

		if ( ! attackArea.empty() ){
			attackArea.render( g, trueX, trueY );
		}
	}

	public synchronized void setImageX( int x ){
		this.x = x;
	}
	
	public synchronized void setImageY( int y ){
		this.y = y;
	}
	
	public synchronized void setOffsetX( int x ){
		this.offsetX = x;
	}
	
	public synchronized void setOffsetY( int y ){
		this.offsetY = y;
	}

	public void addDrawable( JComponent draw ){
		synchronized( drawables ){
			drawables.add( draw );
		}
	}
	
	public void removeDrawable( JComponent draw ){
		synchronized( drawables ){
			drawables.remove( draw );
		}
	}

	private void rest( int m ){
		try{
			Thread.sleep( m );
		} catch ( Exception e ){
		}
	}

	private int previous( int index, int max ){
		int last = index - 1;
		if ( last < 0 ){
			last = max - 1;
		}
		return last;
	}

	private int next( int index, int max ){
		int next = index + 1;
		if ( next >= max ){
			next = 0;
		}
		return next;
	}

	private void updateEvent( AnimationEvent event ){
		event.interact( this );
	}

	/* can be called to step backward through the animation */
	public void previousEvent(){
		synchronized( events ){
			if ( ! events.isEmpty() ){
				eventIndex = previous( eventIndex, events.size() );
				updateEvent( (AnimationEvent) events.get( eventIndex ) );
			}
		}
	}
	
	/* can be called to step foreward through the animation */
	public void nextEvent(){
		synchronized( events ){
			if ( ! events.isEmpty() ){
				eventIndex = next( eventIndex, events.size() );
				updateEvent( (AnimationEvent) events.get( eventIndex ) );
			}
		}
	}

	public void setDelay( int delay ){
		this.delay = delay;
	}

	public int getDelay(){
		return delay;
	}

	public int getDelayTime(){
		return delayTime;
	}

	public void setDelayTime( int i ){
		delayTime = i;
	}

	public void delay(){
		delayTime = getDelay();
	}

	/* if the animation doesnt contain something that will cause delay this
	 * thread will just chew up cpu time
	 */
	public void run(){
		while ( isAlive() ){
			if ( isRunning() && ! events.isEmpty() ){
				nextEvent();
				if ( getDelayTime() != 0 ){
					rest( (int)(getDelayTime() * 1000.0 / 90.0) );
				}
				setDelayTime( 0 );
			} else {
				rest( 10 );
			}
		}
	}

	public void loadData(Token token) throws LoadException {
		if ( ! token.getName().equals( "anim" ) ){
			throw new LoadException( "Starting token is not 'anim'" );
		}
		
		Token nameToken = token.findToken( "name" );
		if ( nameToken != null ){
			setName( nameToken.readString(0) );
		}
		
		/*
		Token typeToken = token.findToken( "type" );
		if ( typeToken != null ){
			for(int i=0; i < typeCombo.getItemCount();++i)
			{
				if(((String)typeCombo.getItemAt(i)).equals(typeToken.readString(0)))
				{
					typeCombo.setSelectedIndex(i);
					type = nameToken.readString(0);
					break;
				}
			}
			
		}
		*/
		
		Token keyToken = token.findToken( "keys" );
		if ( keyToken != null ){
			try{
				for(int i = 0; ; i += 1 ){
					String temp = keyToken.readString(i);
					if ( temp != null ){
						keys.add( temp );
					} else {
						break;
					}
				}
			} catch(Exception e) {
				e.printStackTrace();
			}
		}
		
		Token rangeToken = token.findToken( "range" );
		if ( rangeToken != null ){
			range = rangeToken.readInt(0);
		}
		
		/*
		Token basedirToken = token.findToken( "basedir" );
		if ( basedirToken != null )
		{
			basedirField.setText(basedirToken.readString(0));
			baseDirectory = basedirToken.readString(0);
			DrawState.setCurrentDirList(Animator.dataPath(baseDirectory));
		}
		*/
		
		events.clear();
		for ( Iterator it = token.getTokens().iterator(); it.hasNext(); ){
			Token t = (Token) it.next();
			AnimationEvent ae = EventFactory.getEvent(t.getName());
			if( ae != null ){
				ae.loadToken(t);
				events.add( ae );
			}
		}
	}

	public Token getToken(){
		Token token = new Token();
		token.addToken( new Token( "anim" ) );
		
		token.addToken(new String[]{"name", "\"" + name + "\""});
		if ( ! type.equals("none") ){
			token.addToken(new String[]{"type", type});
		}

		if ( ! keys.isEmpty() ){
			Token keyToken = new Token( "keys" );
			keyToken.addToken( new Token( "keys"));
			for ( Iterator it = keys.iterator(); it.hasNext(); ){
				String key = (String) it.next();
				keyToken.addToken( new Token( key ) );
			}
			token.addToken( keyToken );
		}

		if ( range != 0 ){
			token.addToken(new String[]{"range", Integer.toString(range)});
		}

		if( ! baseDirectory.equals( "" ) ){
			token.addToken(new String[]{"basedir", baseDirectory});
		}

		for ( Iterator it = events.iterator(); it.hasNext(); ){
			AnimationEvent event = (AnimationEvent) it.next();
			token.addToken( event.getToken() );
		}
		
		return token;
	}
}
