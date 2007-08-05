package com.rafkind.paintown.animator;

import java.util.*;

import com.rafkind.paintown.animator.CharacterFrame;

public class CharacterAnimation
{
	// Name
	private String name;
	
	// Type if special
	private String type;
	
	// key sequence (String)
	private Vector keys;
	
	// Range
	private int range;
	
	// Face
	private String face;
	
	// Base dir
	private String baseDirectory;
	
	// Frames
	private Vector frames = new Vector();
	
	public void setName(String n)
	{
		name = n;
	}
	
	public String getName()
	{
		return name;
	}
	
	public void setType(String t)
	{
		type = t;
	}
	
	public String getType()
	{
		return type;
	}
	
	public void addKey(String key)
	{
		keys.addElement(key);
	}
	
	public Vector getKeys()
	{
		return keys;
	}
	
	public void clearKeys()
	{
		keys.clear();
	}
	
	public void setFace(String f)
	{
		face = f;
	}
	
	public String getFace()
	{
		return face;
	}
	
	public void setRange(int r)
	{
		range = r;
	}
	
	public int getRange()
	{
		return range;
	}
	
	public void setBaseDirectory(String b)
	{
		baseDirectory = b;
	}
	
	public String getBaseDirectory()
	{
		return baseDirectory;
	}
	
	public void addFrame(CharacterFrame frame)
	{
		frames.addElement(frame);
	}
	
	public boolean removeFrame(String name)
	{
		Iterator itor = frames.iterator();
		while(itor.hasNext())
		{
			CharacterFrame frame = (CharacterFrame)itor.next();
			if(frame.getName().equals(name))
			{
				frames.removeElement(frame);
				return true;
			}
		}
		return false;
	}
	
	public Vector getFrames()
	{
		return frames;
	}
	
	public String getScript()
	{
		String temp = "";
		
		temp += "(anim \n" +
			"   (name " + name + ") \n";
		if(type.equals(""))temp += "   (type " + type + ") \n";
		temp +=	"   (keys ";
		Iterator kItor = keys.iterator();
		while(kItor.hasNext())
		{
			String key = (String)kItor.next();
			temp += key + " ";
		}
		temp += ") \n";
		if(range!=0)temp += "   (range " + Integer.toString(range) + ") \n";
		if(face.equals(""))temp += "   (face " + face + ") \n";
		Iterator fItor = frames.iterator();
		while(fItor.hasNext())
		{
			CharacterFrame frame = (CharacterFrame)fItor.next();
			temp += frame.getScript();
		}
		temp += "\n";
		
		// Debug
		System.out.println(temp);
		
		return temp;
	}
	
	public CharacterAnimation()
	{
		// Nothing
	}
}
