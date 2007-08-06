package com.rafkind.paintown.animator;

import java.util.*;

import com.rafkind.paintown.Token;

import com.rafkind.paintown.animator.BoundingBox;

public class CharacterFrame
{
	// Private name of image for internal use
	private String name = "";
	
	// Attack data
	public boolean hasAttack;
	public BoundingBox attackArea = new BoundingBox();
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
	public String sound = "";
	
	// Image
	public String image = "";
	
	public CharacterFrame(String imageName)
	{
		name = imageName;
	}
	
	public String getName()
	{
		return name;
	}
	
	public Token getToken()
	{
		Token token = new Token();
		
		if(hasAttack == true)
		{
			Token attackToken = new Token( "attack" );
			attackToken.addToken(new String[]{"x1",Integer.toString(attackArea.x1)});
			attackToken.addToken(new String[]{"y1",Integer.toString(attackArea.y1)});
			attackToken.addToken(new String[]{"x2",Integer.toString(attackArea.x2)});
			attackToken.addToken(new String[]{"y2",Integer.toString(attackArea.y2)});
			attackToken.addToken(new String[]{"damage",Integer.toString(attackDamage)});
			attackToken.addToken(new String[]{"force",Integer.toString(attackForce)});
			
			token.addToken(attackToken);
		}
		
		if(delay != 0)token.addToken(new String[]{"delay",Integer.toString(delay)});
		if(offsetX != 0 || offsetY != 0)token.addToken(new String[]{"offset", Integer.toString(offsetX),Integer.toString(offsetY)});
		if(moveX != 0 || moveY != 0 || moveZ != 0)token.addToken(new String[]{"move", Integer.toString(moveX), Integer.toString(moveY), Integer.toString(moveZ)});
		if(zDistance != 0)token.addToken(new String[]{"z-distance", Integer.toString(zDistance)});
		if(coordX != 0 || coordY != 0 || coordZ != 0)token.addToken(new String[]{"coords", Integer.toString(coordX), Integer.toString(coordY), Integer.toString(coordZ)});
		if(shadowX != 0 || shadowY != 0)token.addToken(new String[]{"shadow", Integer.toString(offsetX), Integer.toString(offsetY)});
		if(sound.equals("") == false)token.addToken(new String[]{"sound", sound});
		if(image.equals("") == false)token.addToken(new String[]{"frame",sound});
		
		return token;
		
		/*
		String text = "";
		
		if(hasAttack)
		{
			text += "(attack\n" +
				"   (x1 " + Integer.toString(attackArea.x1) + ")\n" +
				"   (y1 " + Integer.toString(attackArea.y1) + ")\n" +
				"   (x2 " + Integer.toString(attackArea.x2) + ")\n" +
				"   (y2 " + Integer.toString(attackArea.y2) + ")\n" +
				"   (damage " + Integer.toString(attackDamage) + ")\n" +
				"   (force " + Integer.toString(attackForce) + "))\n";
		}
		
		if(delay != 0)text += "(delay " + Integer.toString(delay) + ")\n";
		if(offsetX != 0 || offsetY != 0)text += "(offset " + Integer.toString(offsetX) + " " + Integer.toString(offsetY) + ")\n";
		if(moveX != 0 || moveY != 0 || moveZ != 0)text += "(move " + Integer.toString(moveX) + " " + Integer.toString(moveY) + " " + Integer.toString(moveZ) + ")\n";
		if(zDistance != 0)text += "(z-distance " + Integer.toString(zDistance) + ")\n";
		if(coordX != 0 || coordY != 0 || coordZ != 0)text += "(coords " + Integer.toString(coordX) + " " + Integer.toString(coordY) + " " + Integer.toString(coordZ) + ")\n";
		if(shadowX != 0 || shadowY != 0)text += "(shadow " + Integer.toString(offsetX) + " " + Integer.toString(offsetY) + ")\n";
		if(sound.equals(""))text += "(sound " + sound + ")\n";
		if(image.equals(""))text += "(frame " + sound + ")\n";
		
		// Debug
		System.out.println(text);
		
		return text;
		*/
	}
}
