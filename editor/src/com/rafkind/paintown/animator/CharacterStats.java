package com.rafkind.paintown.animator;

import java.util.*;

public interface CharacterStats
{
	// Basic character info
	public String name = "";
	public int health = 0;
	public double jumpVelocity = 0;
	public double speed = 0;
	public int shadow = 0;
	
	// Sound data
	public String dieSound = "";
	public String landed = "";
	
	// Bitmaps
	public String icon = "";
	
	// Vector of strings containing filename locations
	public Vector remap = new Vector();
	
	// Vector of CharacterAnimations
	public Vector animations = new Vector();
	
	public AnimationEditor getEditor();
}
