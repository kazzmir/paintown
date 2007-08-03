package com.rafkind.paintown.animator;

import java.util.*;

public abstract class CharacterStats
{
	// Basic character info
	public String name = "";
	public int health;
	public double jumpVelocity;
	public double speed;
	public int shadow;
	
	// Sound data
	public String dieSound = "";
	public String landed = "";
	
	// Bitmaps
	public String icon = "";
	
	// Vector of strings containing filename locations
	public Vector remap = new Vector();
	
	// Vector of CharacterAnimations
	public Vector animations = new Vector();
	
	abstract public AnimationEditor getEditor();
	
	public void saveData()
	{
	}
	
	public void loadData()
	{
	}
	
	public String getScript()
	{
		return new String();
	}
	
	public CharacterStats()
	{
		// Nothing
	}
}
