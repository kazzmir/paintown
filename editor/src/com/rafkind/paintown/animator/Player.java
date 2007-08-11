package com.rafkind.paintown.animator;

import java.util.*;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import java.io.*;

import org.swixml.SwingEngine;
import javax.swing.filechooser.FileFilter;

import com.rafkind.paintown.animator.CharacterStats;
import com.rafkind.paintown.animator.DrawArea;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.RelativeFileChooser;

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
	
	
	private JTextField origMapField;
	private JButton origMapButton;
	private JList remapList;
	private JButton addRemapButton;
	private JButton removeRemapButton;
	
	private JList animList;
	private JButton addAnimButton;
	private JButton editAnimButton;
	private JButton removeAnimButton;
	
	private DrawArea _drawArea;
	
	public SpecialPanel getEditor()
	{	
		return new SpecialPanel((JPanel)playerEditor.getRootComponent(), nameField);
	}
	
	public DrawArea getDrawArea()
	{
		return _drawArea;
	}
	
	public void saveData()
	{
	}
	
	public void loadData()
	{
	}
	
	public Token getToken()
	{
		Token temp = new Token("Character");
		temp.addToken(new Token("Character"));
		temp.addToken(new String[]{"name", "\"" + name + "\""});
		temp.addToken(new String[]{"health", Integer.toString(health)});
		temp.addToken(new String[]{"jump-velocity", Double.toString(jumpVelocity)});
		temp.addToken(new String[]{"speed", Double.toString(speed)});
		temp.addToken(new String[]{"type", "Player"});
		temp.addToken(new String[]{"shadow", Integer.toString(shadow)});
		if(!dieSound.equals(""))temp.addToken(new String[]{"die-sound", dieSound});
		if(!landed.equals(""))temp.addToken(new String[]{"landed", landed});
		if(!icon.equals(""))temp.addToken(new String[]{"icon", icon});
		Iterator mapItor = remap.iterator();
		while(mapItor.hasNext())
		{
			String map = (String)mapItor.next();
			temp.addToken(new String[]{"remap", origMap, map});
		}
		
		Iterator animItor = animations.iterator();
		while(animItor.hasNext())
		{
			CharacterAnimation anim = (CharacterAnimation)animItor.next();
			temp.addToken(anim.getToken());
		}
		
		return temp;
	}
	
	public Player(Animator anim)
	{
		super( anim );
		
		playerEditor = new SwingEngine( "animator/base.xml" );
		
		contextEditor = new SwingEngine ( "animator/context.xml");
		
		//debugSwixml(playerEditor);
		//debugSwixml(contextEditor);
		
		context = (JPanel) playerEditor.find( "context" );
		
		canvas = (JPanel) playerEditor.find( "canvas" );
		
		_drawArea = new DrawArea();
		
		canvas.add(_drawArea);
		
		nameField = (JTextField) contextEditor.find( "name" );
		
		nameField.setText(name);
		
		nameField.getDocument().addDocumentListener(new DocumentListener()
		{
			public void changedUpdate(DocumentEvent e)
			{
				name = nameField.getText();
			}
			public void insertUpdate(DocumentEvent e)
			{
				name = nameField.getText();
			}
			public void removeUpdate(DocumentEvent e)
			{
				name = nameField.getText();
			}
		});
		
		healthSpinner = (JSpinner) contextEditor.find( "health" );
		
		healthSpinner.addChangeListener( new ChangeListener()
		{
			public void stateChanged(ChangeEvent changeEvent)
			{
				health = ((Integer)healthSpinner.getValue()).intValue();
			}
		});
		
		jumpSpinner = (JPanel) contextEditor.find( "jump-velocity" );
		
		final JSpinner jumpSpinTemp = new JSpinner(new SpinnerNumberModel(0, -1000, 1000, .01));
		
		jumpSpinTemp.addChangeListener( new ChangeListener()
		{
			public void stateChanged(ChangeEvent changeEvent)
			{
				jumpVelocity = ((Double)jumpSpinTemp.getValue()).doubleValue();
			}
		});
		
		jumpSpinner.add(jumpSpinTemp);
		
		speedSpinner = (JPanel) contextEditor.find( "speed" );
		
		final JSpinner speedSpinTemp = new JSpinner(new SpinnerNumberModel(0, -1000, 1000, .01));
		
		speedSpinner.add(speedSpinTemp);
		
		speedSpinTemp.addChangeListener( new ChangeListener()
		{
			public void stateChanged(ChangeEvent changeEvent)
			{
				speed = ((Double)speedSpinTemp.getValue()).doubleValue();
			}
		});
		
		shadowSpinner = (JSpinner) contextEditor.find( "shadow" );
		
		shadowSpinner.addChangeListener( new ChangeListener()
		{
			public void stateChanged(ChangeEvent changeEvent)
			{
				shadow = ((Integer)shadowSpinner.getValue()).intValue();
			}
		});
		
		deathSoundField = (JTextField) contextEditor.find( "die-sound" );
		
		deathSoundButton = (JButton) contextEditor.find( "change-die-sound" );
		
		deathSoundButton.addActionListener( new AbstractAction(){
				public void actionPerformed( ActionEvent event ){
					RelativeFileChooser chooser = getNewFileChooser();
					int ret = chooser.open();
					if ( ret == RelativeFileChooser.OK ){
						final String path = chooser.getPath();
						deathSoundField.setText( path );
						dieSound = path;
					}
				}
			});
		
		landingSoundField = (JTextField) contextEditor.find( "land-sound" );
		
		landingSoundButton = (JButton) contextEditor.find( "change-land-sound" );
		
		landingSoundButton.addActionListener( new AbstractAction(){
				public void actionPerformed( ActionEvent event ){
					RelativeFileChooser chooser = getNewFileChooser();
					int ret = chooser.open();
					if ( ret == RelativeFileChooser.OK ){
						final String path = chooser.getPath();
						landingSoundField.setText( path );
						landed = path;
					}
				}
			});
		
		iconField = (JTextField) contextEditor.find( "icon" );
		
		iconButton = (JButton) contextEditor.find( "change-icon" );
		
		iconButton.addActionListener( new AbstractAction(){
				public void actionPerformed( ActionEvent event ){
					RelativeFileChooser chooser = getNewFileChooser();
					int ret = chooser.open();
					if ( ret == RelativeFileChooser.OK ){
						final String path = chooser.getPath();
						iconField.setText( path );
						icon = path;
					}
				}
			});
			
		origMapField = (JTextField) contextEditor.find( "original-map" );
		
		origMapButton = (JButton) contextEditor.find( "change-origmap" );
		
		origMapButton.addActionListener( new AbstractAction(){
				public void actionPerformed( ActionEvent event ){
					RelativeFileChooser chooser = getNewFileChooser();
					int ret = chooser.open();
					if ( ret == RelativeFileChooser.OK ){
						final String path = chooser.getPath();
						origMapField.setText( path );
						origMap = path;
					}
				}
			});
		
		remapList = (JList) contextEditor.find( "remaps" );
		
		addRemapButton = (JButton) contextEditor.find( "add-remap" );
		
		addRemapButton.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				RelativeFileChooser chooser = getNewFileChooser();
				int ret = chooser.open();
				if ( ret == RelativeFileChooser.OK ){
					final String path = chooser.getPath();
					remap.addElement( path );
					remapList.setListData(remap);
				}
			}
		});
		
		removeRemapButton = (JButton) contextEditor.find( "remove-remap" );
		
		removeRemapButton.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				String temp = (String)remap.elementAt(remapList.getSelectedIndex());
				removeMap(temp);
				remapList.setListData(remap);
			}
		});
		
		animList = (JList) contextEditor.find( "anims");
		
		animList.addMouseListener( new MouseAdapter()
		{
			public void mouseClicked(MouseEvent event)
			{
				//animList.setListData(animations);
				_drawArea = ((CharacterAnimation)animList.getSelectedValue()).getDrawArea();
			}
		});
		
		animList.setCellRenderer(new DefaultListCellRenderer() {
			public Component getListCellRendererComponent(
				JList list,
				Object value,
				int index,
				boolean isSelected,
				boolean cellHasFocus)
			{
				setText(((CharacterAnimation)value).getName());
				setBackground(isSelected ? Color.gray : Color.white);
				setForeground(isSelected ? Color.white : Color.black);
				return this;
			}
			});
		
		addAnimButton = (JButton) contextEditor.find( "add-anim" );
		
		addAnimButton.addActionListener( new AbstractAction()
		{
			public void actionPerformed( ActionEvent event )
			{
				createAnimation();
				animList.setListData(animations);
				
				System.out.println(getToken().toString());
			}
		});
		
		editAnimButton = (JButton) contextEditor.find( "edit-anim" );
		
		editAnimButton.addActionListener( new AbstractAction()
		{
			public void actionPerformed( ActionEvent event )
			{
				editAnimation(animList.getSelectedIndex());
			}
		});
		
		removeAnimButton = (JButton) contextEditor.find( "remove-anim" );
		
		removeAnimButton.addActionListener( new AbstractAction()
		{
			public void actionPerformed( ActionEvent event )
			{
				removeAnimation(animList.getSelectedIndex());
				animList.setListData(animations);
			}
		});
		
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
