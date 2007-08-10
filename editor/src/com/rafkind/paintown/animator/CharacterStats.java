package com.rafkind.paintown.animator;

import java.util.*;
import java.io.*;
import javax.swing.*;

import com.rafkind.paintown.*;
import com.rafkind.paintown.animator.Animator;
import com.rafkind.paintown.animator.SpecialPanel;

public abstract class CharacterStats
{
	// Animator for stuff
	private Animator _animator;
	
	// Basic character info
	protected String name = "";
	protected int health;
	protected double jumpVelocity;
	protected double speed;
	protected int shadow;
	
	// Sound data
	protected String dieSound = "";
	protected String landed = "";
	
	// Bitmaps
	protected String icon = "";
	
	// Original Map for remaps
	protected String origMap = "";
	
	// Vector of strings containing filename locations
	protected Vector remap = new Vector();
	
	// Vector of CharacterAnimations
	protected Vector animations = new Vector();
	
	public String getName()
	{
		return name;
	}
	
	public void setName(String n)
	{
		name = n;
	}
	
	public int getHealth()
	{
		return health;
	}
	
	public void setHealth(int h)
	{
		health = h;
	}
	
	public double getJumpVelocity()
	{
		return jumpVelocity;
	}
	
	public void setJumpVelocity(int j)
	{
		jumpVelocity = j;
	}
	
	public double getSpeed()
	{
		return speed;
	}
	
	public void setSpeed(int s)
	{
		speed = s;
	}
	
	public int getShadow()
	{
		return shadow;
	}
	
	public void setShadow(int s)
	{
		shadow = s;
	}
	
	public String getDieSound()
	{
		return dieSound;
	}
	
	public void setDieSound(String d)
	{
		dieSound = d;
	}
	
	public String getLanded()
	{
		return landed;
	}
	
	public void setLanded(String l)
	{
		landed = l;
	}
	
	public String getIcon()
	{
		return icon;
	}
	
	public void setIcon(String i)
	{
		icon = i;
	}
	
	public void setOrigMap(String map)
	{
		origMap = map;
	}
	
	public String getOrigMap()
	{
		return origMap;
	}
	
	public void addMap(String map)
	{
		remap.addElement(map);
	}
	
	public boolean removeMap(int index)
	{
		String temp = (String)animations.elementAt(index);
		return remap.removeElement(temp);
	}
	
	public boolean removeMap(String map)
	{
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
	
	public void createAnimation()
	{
		CharacterAnimation temp = new CharacterAnimation(_animator);
		_animator.addNewTab(temp.getEditor(), temp.getName());
		animations.addElement(temp);
	}
	
	public void editAnimation(int index)
	{
		CharacterAnimation temp = (CharacterAnimation)animations.elementAt(index);
		_animator.addNewTab(temp.getEditor(), temp.getName());
	}
	
	public boolean removeAnimation(int index)
	{
		CharacterAnimation temp = (CharacterAnimation)animations.elementAt(index);
		return animations.removeElement(temp);
	}
	
	public boolean removeAnimation(CharacterAnimation anim)
	{
		return animations.removeElement(anim);
	}
	
	public Vector getAnimations()
	{
		return animations;
	}
	
	// This is so we can add tabs of animation editors to the main window
	private void addAnimator(Animator anim)
	{
		_animator = anim;
	}
	
	public RelativeFileChooser getNewFileChooser()
	{
		return _animator.getNewFileChooser();
	}
	
	abstract public SpecialPanel getEditor();
	
	abstract public void saveData();
	
	abstract public void loadData();
	
	abstract public Token getToken();
	
	public CharacterStats(Animator anim)
	{
		addAnimator(anim);
		
		name = "New Character";
	}
}
