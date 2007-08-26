package com.rafkind.paintown.animator;

import java.util.*;
import java.io.*;
import javax.swing.*;

import com.rafkind.paintown.*;
import com.rafkind.paintown.exception.*;

public class CharacterStats extends BasicObject {
	protected int health;
	protected double jumpVelocity;
	protected double speed;
	protected int shadow;

	private List updates;
	
	// Sound data
	protected String dieSound = "";
	protected String hitSound = "";
	protected String landed = "";
	
	// Bitmaps
	protected String icon = "";
	
	// Original Map for remaps
	protected String origMap = "";
	
	// Vector of strings containing filename locations
	protected Vector remap = new Vector();
	
	// Vector of CharacterAnimations
	protected Vector animations = new Vector();

	public CharacterStats( String name ){
		this.name = name;
		updates = new ArrayList();
	}

	public CharacterStats( String name, File f ) throws LoadException {
		this( name );
		loadData( f );
	}
	
	public int getHealth(){
		return health;
	}
	
	public void setHealth(int h){
		health = h;
	}
	
	public double getJumpVelocity(){
		return jumpVelocity;
	}
	
	public void setJumpVelocity( double j ){
		jumpVelocity = j;
	}
	
	public double getSpeed(){
		return speed;
	}
	
	public void setSpeed( double s){
		speed = s;
	}
	
	public int getShadow(){
		return shadow;
	}
	
	public void setShadow(int s){
		shadow = s;
	}
	
	public String getDieSound(){
		return dieSound;
	}
	
	public void setDieSound(String d){
		dieSound = d;
	}

	public String getHitSound(){
		return hitSound;
	}

	public void setHitSound( String s ){
		hitSound = s;
	}
	
	public String getLanded(){
		return landed;
	}
	
	public void setLanded(String l){
		landed = l;
	}
	
	public String getIcon(){
		return icon;
	}
	
	public void setIcon(String i){
		icon = i;
	}
	
	public void setOriginalMap(String map){
		origMap = map;
	}
	
	public String getOriginalMap(){
		return origMap;
	}

	public void addAnimationUpdate( Lambda1 update ){
		updates.add( update );
	}
	
	public void removeAnimationUpdate( Lambda1 update ){
		updates.remove( update );
	}

	public Vector getRemaps(){
		return remap;
	}
	
	public void addMap(String map){
		remap.addElement(map);
	}
	
	public void removeMap( int index ){
		remap.remove( index );
	}

	public String getMap( int index ){
		return (String) remap.get( index );
	}
	
	public boolean removeMap(String map){
		Iterator itor = remap.iterator();
		while ( itor.hasNext() ){
			String tempMap = (String)itor.next();
			if ( tempMap.equals( map ) ){
				remap.removeElement( tempMap );
				return true;
			}
		}
		return false;
	}
	
	/*
	public void createAnimation(){
		Animation ani = new Animation();
		new Thread( ani ).start();
		animations.addElement( ani );
		editAnimation( animations.size() - 1 );
		updateAnimationListeners();
	}
	*/
	
	/*
	public void editAnimation(int index){
		Animation temp = (Animation) animations.elementAt( index );
		CharacterAnimation edit = new CharacterAnimation( this, temp );
		animator.addNewTab( edit.getEditor(), temp.getName());
		// edit.fixBaseDirectory();
	}
	*/

	public void updateAnimationListeners(){
		for ( Iterator it = updates.iterator(); it.hasNext(); ){
			Lambda1 update = (Lambda1) it.next();
			try{
				update.invoke( this );
			} catch ( Exception e ){
				e.printStackTrace();
			}
		}
	}
	
	public void removeAnimation( int index ){
		Animation temp = (Animation)animations.elementAt(index);
		animations.removeElement(temp);
		updateAnimationListeners();

	}
	
	public void removeAnimation(Animation anim){
		removeAnimation( animations.indexOf( anim ) );
	}

	public void addAnimation( Animation a ){
		animations.add( a );
	}

	public Vector getAnimations(){
		return animations;
	}

	public Animation getAnimation( int i ){
		return (Animation) getAnimations().get( i );
	}
	
	public Token getToken(){
		Token temp = new Token("character");
		temp.addToken(new Token("character"));
		temp.addToken(new String[]{"name", "\"" + name + "\""});
		temp.addToken(new String[]{"health", Integer.toString(health)});
		temp.addToken(new String[]{"jump-velocity", Double.toString(jumpVelocity)});
		temp.addToken(new String[]{"speed", Double.toString(speed)});
		temp.addToken(new String[]{"type", "Player"});
		temp.addToken(new String[]{"shadow", Integer.toString(shadow)});
		if ( ! getDieSound().equals( "" ) ){
			temp.addToken(new String[]{"die-sound", getDieSound() });
		}

		if ( ! getHitSound().equals( "" ) ){
			temp.addToken(new String[]{"hit-sound", getHitSound() });
		}

		if(!landed.equals(""))temp.addToken(new String[]{"landed", landed});
		if(!icon.equals(""))temp.addToken(new String[]{"icon", icon});
		Iterator mapItor = remap.iterator();
		while(mapItor.hasNext()){
			String map = (String)mapItor.next();
			temp.addToken(new String[]{"remap", origMap, map});
		}
		
		Iterator animItor = animations.iterator();
		while(animItor.hasNext()){
			Animation anim = (Animation)animItor.next();
			temp.addToken(anim.getToken());
		}
		
		return temp;
	}

	public void saveData() throws LoadException {
		try{
			FileOutputStream out = new FileOutputStream( getPath() );
			PrintStream printer = new PrintStream( out );
			printer.print( getToken().toString() );
			printer.print( "\n" );
			out.close();
			System.out.println( getToken().toString() );
		} catch ( Exception e ){
			throw new LoadException( "Couldn't save!" );
		}
	}

	public void loadData(File f) throws LoadException {
		TokenReader reader = new TokenReader( f );
		Token head = reader.nextToken();
		
		if ( ! head.getName().equals( "character" ) ){
			throw new LoadException( "Starting token is not 'character'" );
		}
		
		setPath(f);
		
		Token nameToken = head.findToken( "name" );
		if ( nameToken != null ){
			name = nameToken.readString(0);
		}
		
		Token healthToken = head.findToken( "health" );
		if ( healthToken != null ){
			health = healthToken.readInt(0);
		}
		
		Token jumpToken = head.findToken( "jump-velocity" );
		if ( jumpToken != null ){
			jumpVelocity = jumpToken.readDouble(0);
		}
		
		Token speedToken = head.findToken( "speed" );
		if ( speedToken != null ){
			speed = speedToken.readDouble(0);
		}
		
		Token shadowToken = head.findToken( "shadow" );
		if ( shadowToken != null ){
			shadow = shadowToken.readInt(0);
		}
		
		Token diesoundToken = head.findToken( "die-sound" );
		if ( diesoundToken != null ){
			setDieSound( diesoundToken.readString(0) );
		}

		Token hitsoundToken = head.findToken( "hit-sound" );
		if ( hitsoundToken != null ){
			String path = hitsoundToken.readString( 0 );
			setHitSound( path );
		}
		
		Token landedToken = head.findToken( "landed" );
		if ( landedToken != null ){
			landed = landedToken.readString(0);
		}
		
		Token iconToken = head.findToken( "icon" );
		if ( iconToken != null ){
			icon = iconToken.readString(0);
		}
		
		for ( Iterator it = head.findTokens( "remap" ).iterator(); it.hasNext(); ){
			Token t = (Token) it.next();
			origMap = t.readString(0);
			remap.addElement(t.readString(1));
		}

		for ( Iterator it = head.findTokens( "anim" ).iterator(); it.hasNext(); ){
			Token t = (Token) it.next();
			Animation anim = new Animation( t );
			addAnimation( anim );
			new Thread( anim ).start();
		}
	}
}
