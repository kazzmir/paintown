package com.rafkind.paintown.animator;

import java.util.*;
import javax.swing.*;
import java.awt.*;
import javax.swing.*;

import org.swixml.SwingEngine;

import com.rafkind.paintown.animator.CharacterStats;
import com.rafkind.paintown.animator.DrawArea;
import com.rafkind.paintown.Token;

public final class Player extends CharacterStats
{
	public JPanel getEditor()
	{
		final SwingEngine playerEditor = new SwingEngine( "animator/base.xml" );
		
		final JPanel context = (JPanel) playerEditor.find( "context" );
		
		final JPanel drawArea = (JPanel) playerEditor.find( "canvas" );
		
		final DrawArea canvas = new DrawArea();
		
		drawArea.add(canvas);
		
		final SwingEngine contextEditor = new SwingEngine ( "animator/context.xml");
		
		context.add((JComponent)contextEditor.getRootComponent());
		
		return (JPanel) playerEditor.getRootComponent();
	}
	
	public void saveData()
	{
	}
	
	public void loadData()
	{
	}
	
	public Token getToken()
	{
		return new Token();
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
		String[] bleh = new String[]{"delay", "5"};
		anim.addEvent(bleh);
		anim.addEvent(new String[]{"frame", "someplace"});
		
		System.out.println( anim.getToken().toString() );
		
		anim.removeEvent(bleh);
		System.out.println( anim.getToken().toString() );
	}
}
