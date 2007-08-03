package com.rafkind.paintown.animator;

import java.util.*;

public class CharacterStats
{
	// Basic character info
	public String name = "";
	public int health;
	public double jumpVelocity;
	public double speed;
	public int shadow;
	public int type;
	
	// Since we have no enum due to 1.4 ::)
	public final static int PlayerType = 0;
	public final static int EnemyType = 1;
	public final static int ItemType = 2;
	// Add more later ?
	
	// Sound data
	public String dieSound = "";
	public String landed = "";
	
	// Bitmaps
	public String icon = "";
	
	// Vector of strings containing filename locations
	public Vector remap = new Vector();
	
	// Vector of CharacterAnimations
	public Vector animations = new Vector();
	
	public CharacterStats()
	{
		// Nothing
	}
}
