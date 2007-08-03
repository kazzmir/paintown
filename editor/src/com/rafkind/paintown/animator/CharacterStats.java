package com.rafkind.paintown.animator;

import java.util.*;

public class CharacterStats
{
	// Basic character info
	public String name = "";
	public int health = 0;
	public double jump-velocity = 0;
	public double speed = 0;
	public int shadow = 0;
	public int type = 0;
	public final static int PlayerType = 0;
	public final static int EnemyType = 1;
	public final static int ItemType = 2;
	// Add more later ?
	
	// Sound data
	public String die-sound = "";
	public String landed = "";
	
	// Bitmaps
	public String icon = "";
	public Vector remap = "";
	
	public Vector animations;
	
	public CharacterStats()
	{
		// Nothing
	}
}
