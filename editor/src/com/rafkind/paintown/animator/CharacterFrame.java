package com.rafkind.paintown.animator;

import java.util.*;
import com.rafkind.paintown.animator.BoundingBox;

public class CharacterFrame
{
	// Attack data
	public boolean hasAttack;
	public BoundingBox attackArea;
	public int attackDamage;
	public int attackForce;
	
	// Delay
	public int delay;
	
	// Offsets
	public int offsetX;
	public int offsetY;
	
	// Move
	public int moveX;
	public int moveY;
	public int moveZ;
	
	// Z-Distance
	public int zDistance;
	
	// Coords
	public int coordX;
	public int coordY;
	public int coordZ;
	
	// Shadow
	public int shadowX;
	public int shadowY;
	
	// Sound
	public String sound;
	
	public CharacterFrame()
	{
		// Nothing
	}
}
