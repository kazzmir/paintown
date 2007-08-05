package com.rafkind.paintown.animator;

import java.util.*;
import javax.swing.*;

public abstract class CharacterStats
{
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
	
	public void addMap(String map)
	{
		remap.addElement(map);
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
	
	public void addAnimation(CharacterAnimation anim)
	{
		animations.addElement(anim);
	}
	
	public boolean removeAnimation(String anim)
	{
		Iterator itor = animations.iterator();
		while(itor.hasNext())
		{
			CharacterAnimation tempAnim = (CharacterAnimation)itor.next();
			if(tempAnim.getName().equals(anim))
			{
				animations.removeElement(tempAnim);
				return true;
			}
		}
		return false;
	}
	
	abstract public JPanel getEditor();
	
	abstract public void saveData();
	
	abstract public void loadData();
	
	abstract public String getScript();
	
	public CharacterStats()
	{
		// Nothing
	}
}
