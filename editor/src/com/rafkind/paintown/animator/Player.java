package com.rafkind.paintown.animator;

import java.util.*;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

import org.swixml.SwingEngine;

import com.rafkind.paintown.animator.CharacterStats;
import com.rafkind.paintown.animator.DrawArea;
import com.rafkind.paintown.Token;

public final class Player extends CharacterStats
{
	private SwingEngine playerEditor;
	private SwingEngine contextEditor;
	private JPanel context;
	private JPanel canvas;
	private JTextField nameField;
	private JSpinner healthSpinner;
	private JPanel jumpSpinner;
	private JPanel speedSpinner;
	private JSpinner shadowSpinner;
	private JTextField deathSoundField;
	private JButton deathSoundButton;
	private JTextField landingSoundField;
	private JButton landingSoundButton;
	private JTextField iconField;
	private JButton iconButton;
	
	private JButton addRemapButton;
	private JButton removeRemapButton;
	
	private JButton addAnimButton;
	private JButton editAnimButton;
	private JButton removeAnimButton;
	
	public JPanel getEditor()
	{	
		
		final DrawArea area = new DrawArea();
		
		canvas.add(area);
		
		
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
	
	public Player(Animator anim)
	{
		super( anim );
		
		playerEditor = new SwingEngine( "animator/base.xml" );
		
		contextEditor = new SwingEngine ( "animator/context.xml");
		
		debugSwixml(playerEditor);
		debugSwixml(contextEditor);
		
		context = (JPanel) playerEditor.find( "context" );
		
		canvas = (JPanel) playerEditor.find( "canvas" );
		
		nameField = (JTextField) contextEditor.find( "name" );
		
		healthSpinner = (JSpinner) contextEditor.find( "health" );
		
		jumpSpinner = (JPanel) contextEditor.find( "jump-velocity" );
		
		jumpSpinner.add(new JSpinner(new SpinnerNumberModel(0, -1000, 1000, .01)));
		
		speedSpinner = (JPanel) contextEditor.find( "speed" );
		
		speedSpinner.add(new JSpinner(new SpinnerNumberModel(0, -1000, 1000, .01)));
		
		shadowSpinner = (JSpinner) contextEditor.find( "shadow" );
		
		deathSoundField = (JTextField) contextEditor.find( "die-sound" );
		
		deathSoundButton = (JButton) contextEditor.find( "change-die-sound" );
		
		landingSoundField = (JTextField) contextEditor.find( "land-sound" );
		
		landingSoundButton = (JButton) contextEditor.find( "change-land-sound" );
		
		iconField = (JTextField) contextEditor.find( "icon" );
		
		iconButton = (JButton) contextEditor.find( "change-icon" );
		
		addRemapButton = (JButton) contextEditor.find( "add-remap" );
		
		removeRemapButton = (JButton) contextEditor.find( "remove-remap" );
		
		addAnimButton = (JButton) contextEditor.find( "add-anim" );
		
		addAnimButton.addActionListener( new AbstractAction()
		{
			public void actionPerformed( ActionEvent event )
			{
				// This will need to change to a factory or something
				createAnimation();
			}
		});
		
		editAnimButton = (JButton) contextEditor.find( "edit-anim" );
		
		removeAnimButton = (JButton) contextEditor.find( "remove-anim" );
		
		context.add((JComponent)contextEditor.getRootComponent());
	}
	
	
	
	private void debugSwixml( SwingEngine engine ){
		Map all = engine.getIdMap();
		System.out.println( "Debugging swixml" );
		for ( Iterator it = all.entrySet().iterator(); it.hasNext(); ){
			Map.Entry entry = (Map.Entry) it.next();
			System.out.println( "Id: " + entry.getKey() + " = " + entry.getValue() );
		}
	}
}
