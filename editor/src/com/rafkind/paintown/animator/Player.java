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
	}
}
