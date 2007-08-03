package com.rafkind.paintown.animator;

import java.util.*;

import com.rafkind.paintown.animator.CharacterFrame;

public class CharacterAnimation
{
	// Name
	public String name;
	
	// Type if special
	public String type;
	
	// key sequence
	public Vector keys;
	
	// Range
	public int range;
	
	// Base dir
	public String baseDirectory;
	
	// Frames
	public Vector frames = new Vector();
	
	public CharacterAnimation()
	{
		// Nothing
	}
}
