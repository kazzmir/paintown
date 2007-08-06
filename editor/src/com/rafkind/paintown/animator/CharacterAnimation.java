package com.rafkind.paintown.animator;

import java.util.*;
import javax.swing.*;
import java.awt.*;
import javax.swing.*;

import org.swixml.SwingEngine;

import com.rafkind.paintown.Token;

public class CharacterAnimation
{
	// Name
	private String name = "";
	
	// Type if special
	private String type = "";
	
	// key sequence (String)
	private Vector keys = new Vector();
	
	// Range
	private int range;
	
	// Face
	private String face = "";
	
	// Base dir
	private String baseDirectory = "";
	
	// Events
	private Vector events = new Vector();
	
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
	
	public void addEvent(String[] event)
	{
		events.addElement(event);
	}
	
	public void removeEvent(String[] event)
	{
		events.removeElement(event);
	}
	
	public Vector getEvents()
	{
		return events;
	}
	
	public Token getToken()
	{
		Token token = new Token();
		token.addToken( new Token( "anim" ) );
		
		token.addToken(new String[]{"name", name});
		if(type.equals("") == false)token.addToken(new String[]{"type", type});
		Token keyToken = new Token( "keys" );
		keyToken.addToken( new Token( "keys"));
		Iterator kItor = keys.iterator();
		while(kItor.hasNext())
		{
			String key = (String)kItor.next();
			keyToken.addToken(new Token(key));
		}
		token.addToken(keyToken);
		if(range!=0)token.addToken(new String[]{"range", Integer.toString(range)});
		if(face.equals("") == false)token.addToken(new String[]{"face", face});
		Iterator fItor = events.iterator();
		while(fItor.hasNext())
		{
			String[] args = (String[])fItor.next();
			token.addToken(args);
		}
		
		return token;
	}
	
	public JPanel getEditor()
	{
		final SwingEngine playerEditor = new SwingEngine( "animator/player.xml" );
		
		final JPanel context = (JPanel) playerEditor.find( "context" );
		
		final SwingEngine contextEditor = new SwingEngine ( "animator/animation.xml");
		
		context.add((JComponent)contextEditor.getRootComponent());
		
		return (JPanel) playerEditor.getRootComponent();
	}
	
	public CharacterAnimation()
	{
		// Nothing
	}
}
