package com.rafkind.paintown.animator;

import java.util.*;
import java.io.*;
import javax.swing.*;

import com.rafkind.paintown.*;
import com.rafkind.paintown.exception.*;
import com.rafkind.paintown.animator.Animator;
import com.rafkind.paintown.animator.Animation;
import com.rafkind.paintown.animator.SpecialPanel;
import com.rafkind.paintown.animator.DrawArea;

public abstract class CharacterStats{
	// Animator for stuff
	private Animator animator;
	
	// Path info
	private File file;
	
	// Basic character info
	protected String name = "";
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

	public CharacterStats( Animator anim ){
		addAnimator(anim);
		name = "New Character";
		updates = new ArrayList();
	}
	
	public File getPath(){
		return file;
	}
	
	public void setPath(File f){
		file = f;
	}
	
	public String getName(){
		return name;
	}
	
	public void setName(String n){
		name = n;
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
	
	public void setJumpVelocity(int j){
		jumpVelocity = j;
	}
	
	public double getSpeed(){
		return speed;
	}
	
	public void setSpeed(int s){
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
	
	public void setOrigMap(String map){
		origMap = map;
	}
	
	public String getOrigMap(){
		return origMap;
	}

	public void addAnimationUpdate( Lambda1 update ){
		updates.add( update );
	}
	
	public void removeAnimationUpdate( Lambda1 update ){
		updates.remove( update );
	}
	
	public void addMap(String map){
		remap.addElement(map);
	}
	
	public boolean removeMap(int index){
		String temp = (String)animations.elementAt(index);
		return remap.removeElement(temp);
	}
	
	public boolean removeMap(String map){
		Iterator itor = remap.iterator();
		while(itor.hasNext())
		{
			String tempMap = (String)itor.next();
			if(tempMap.equals(map))
			{
				remap.removeElement(tempMap);
				return true;
			}
		}
		return false;
	}
	
	public void createAnimation(){
		Animation ani = new Animation();
		new Thread( ani ).start();
		animations.addElement( ani );
		editAnimation( animations.size() - 1 );
		updateAnimationListeners();
		/*
		CharacterAnimation temp = new CharacterAnimation( this, ani );
		_animator.addNewTab( temp.getEditor(), ani.getName() );
		*/
	}
	
	public void editAnimation(int index){
		Animation temp = (Animation) animations.elementAt( index );
		CharacterAnimation edit = new CharacterAnimation( this, temp );
		animator.addNewTab( edit.getEditor(), temp.getName());
		// edit.fixBaseDirectory();
	}

	private void updateAnimationListeners(){
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
	
	public Vector getAnimations(){
		return animations;
	}
	
	// This is so we can add tabs of animation editors to the main window
	private void addAnimator(Animator anim){
		animator = anim;
	}
	
	abstract public SpecialPanel getEditor();
	
	abstract public DrawArea getDrawArea();
	
	abstract public void saveData( File f ) throws LoadException;
	
	abstract public void loadData( File f ) throws LoadException;
	
	abstract public Token getToken();
	
}
