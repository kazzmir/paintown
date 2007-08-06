package com.rafkind.paintown.animator;

import java.util.*;
import javax.swing.*;
import java.awt.*;
import javax.swing.*;

import org.swixml.SwingEngine;

import com.rafkind.paintown.animator.CharacterStats;

public final class Player extends CharacterStats
{
	public JPanel getEditor()
	{
		final SwingEngine engine = new SwingEngine( "animator/player.xml" );
		
		return (JPanel) engine.getRootComponent();
	}
	
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
	
	public Player()
	{
		// Nothing
		
		CharacterAnimation anim = new CharacterAnimation();
		anim.setName("dude");
		anim.setType("fs");
		anim.addKey("left");
		anim.addKey("right");
		anim.setFace("reverse");
		anim.setRange(1);
		
		CharacterFrame frame = new CharacterFrame("bs");
		frame.delay = 2;
		frame.image = "someplace";
		
		anim.addFrame(frame);
		
		System.out.println( anim.getToken().toString() );
	}
}
