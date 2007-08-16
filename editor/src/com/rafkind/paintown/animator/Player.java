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
import com.rafkind.paintown.exception.*;
import com.rafkind.paintown.*;

public final class Player extends CharacterStats
{
	private SwingEngine playerEditor;
	private SwingEngine contextEditor;
	private SwingEngine controlEditor;
	private JPanel context;
	private JPanel canvas;
	private JPanel controls;
	private JTextField nameField;
	private JSpinner healthSpinner;
	private JPanel jumpSpinner;
	private JSpinner jumpSpinner2;
	private JPanel speedSpinner;
	private JSpinner speedSpinner2;
	private JSpinner shadowSpinner;
	private JTextField deathSoundField;
	private JButton deathSoundButton;
	private JTextField landingSoundField;
	private JButton landingSoundButton;
	private JTextField iconField;
	private JButton iconButton;

	private Animation currentAnimation;
	private Thread currentThread;
	
	
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
	
	private JButton displayToken;
	private JButton stopAnim;
	private JButton playAnim;
	
	public SpecialPanel getEditor()
	{	
		return new SpecialPanel((JPanel)playerEditor.getRootComponent(), nameField,this);
	}
	
	public DrawArea getDrawArea()
	{
		return _drawArea;
	}
	
	public void saveData(File f) throws LoadException
	{
		setPath(f);
		try
		{
			FileOutputStream out = new FileOutputStream( f );
			new PrintStream( out ).print( getToken().toString() );
			out.close();
			System.out.println( getToken().toString() );
		}
		catch(Exception e)
		{
			throw new LoadException( "Couldn't save!" );
		}
	}
	
	public void loadData(File f) throws LoadException
	{
		TokenReader reader = new TokenReader( f );
		Token head = reader.nextToken();
		
		if ( ! head.getName().equals( "character" ) ){
			throw new LoadException( "Starting token is not 'character'" );
		}
		
		setPath(f);
		
		Token nameToken = head.findToken( "name" );
		if ( nameToken != null )
		{
			nameField.setText(nameToken.readString(0));
			name = nameToken.readString(0);
		}
		
		Token healthToken = head.findToken( "health" );
		if ( healthToken != null )
		{
			healthSpinner.setValue(new Integer(healthToken.readInt(0)));
			health = healthToken.readInt(0);
		}
		
		Token jumpToken = head.findToken( "jump-velocity" );
		if ( jumpToken != null )
		{
			jumpSpinner2.setValue(new Double(jumpToken.readDouble(0)));
			jumpVelocity = jumpToken.readDouble(0);
		}
		
		Token speedToken = head.findToken( "speed" );
		if ( speedToken != null )
		{
			speedSpinner2.setValue(new Double(speedToken.readDouble(0)));
			speed = speedToken.readDouble(0);
		}
		
		Token shadowToken = head.findToken( "shadow" );
		if ( shadowToken != null )
		{
			shadowSpinner.setValue(new Integer(shadowToken.readInt(0)));
			shadow = shadowToken.readInt(0);
		}
		
		Token diesoundToken = head.findToken( "die-sound" );
		if ( diesoundToken != null )
		{
			deathSoundField.setText(diesoundToken.readString(0));
			dieSound = diesoundToken.readString(0);
		}
		
		Token landedToken = head.findToken( "landed" );
		if ( landedToken != null )
		{
			landingSoundField.setText(landedToken.readString(0));
			landed = landedToken.readString(0);
		}
		
		Token iconToken = head.findToken( "icon" );
		if ( iconToken != null )
		{
			iconField.setText(iconToken.readString(0));
			icon = iconToken.readString(0);
		}
		
		for ( Iterator it = head.findTokens( "remap" ).iterator(); it.hasNext(); ){
			Token t = (Token) it.next();
			origMapField.setText(t.readString( 0 ));
			origMap = t.readString(0);
			remap.addElement(t.readString(1));
		}
		remapList.setListData(remap);
		
		for ( Iterator it = head.findTokens( "anim" ).iterator(); it.hasNext(); ){
			Token t = (Token) it.next();
			Animation charanim = new Animation();
			charanim.loadData(t);
			animations.addElement(charanim);
			new Thread( charanim ).start();
		}
		animList.setListData(animations);
		
	}
	
	public Token getToken()
	{
		Token temp = new Token("character");
		temp.addToken(new Token("character"));
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
		
		controlEditor = new SwingEngine( "animator/controls.xml" );
		
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
		
		jumpSpinner2 = new JSpinner(new SpinnerNumberModel(0, -1000, 1000, .01));
		
		jumpSpinner2.addChangeListener( new ChangeListener()
		{
			public void stateChanged(ChangeEvent changeEvent)
			{
				jumpVelocity = ((Double)jumpSpinner2.getValue()).doubleValue();
			}
		});
		
		jumpSpinner.add(jumpSpinner2);
		
		speedSpinner = (JPanel) contextEditor.find( "speed" );
		
		speedSpinner2 = new JSpinner(new SpinnerNumberModel(0, -1000, 1000, .01));
		
		speedSpinner.add(speedSpinner2);
		
		speedSpinner2.addChangeListener( new ChangeListener()
		{
			public void stateChanged(ChangeEvent changeEvent)
			{
				speed = ((Double)speedSpinner2.getValue()).doubleValue();
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
					RelativeFileChooser chooser = Animator.getNewFileChooser();
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
					RelativeFileChooser chooser = Animator.getNewFileChooser();
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
					RelativeFileChooser chooser = Animator.getNewFileChooser();
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
					RelativeFileChooser chooser = Animator.getNewFileChooser();
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
				RelativeFileChooser chooser = Animator.getNewFileChooser();
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
		
		animList.addMouseListener( new MouseAdapter() {
			public void mouseClicked(MouseEvent e) {
				if (e.getClickCount() == 2) {
					int index = animList.locationToIndex(e.getPoint());
					editAnimation(index);
				}
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
				setText(((Animation)value).getName());
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
				
				//System.out.println(getToken().toString());
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
		
		
		controls = (JPanel) playerEditor.find( "controls" );
		
		displayToken = (JButton) controlEditor.find( "token" );
		
		displayToken.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				final JDialog tempDiag = new JDialog();
				tempDiag.setSize(400,400);
				final JTextArea tempText = new JTextArea();
				final JScrollPane tempPane = new JScrollPane(tempText);
				tempDiag.add(tempPane);
				tempText.setText(getToken().toString());
				tempDiag.show();
			}
		});
		
		stopAnim = (JButton) controlEditor.find( "stop" );
		stopAnim.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				_drawArea.unanimate();
				if ( currentAnimation != null ){
					currentAnimation.stopRunning();
				}
			}
		});

		playAnim = (JButton) controlEditor.find( "play" );
		playAnim.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				if( animList.getSelectedValue() != null ){
					if ( currentAnimation != null ){
						currentAnimation.stopRunning();
					}
					currentAnimation = (Animation) animList.getSelectedValue();
					_drawArea.animate( currentAnimation );
					currentAnimation.startRunning();
				}
			}
		});
		
		controls.add((JComponent)controlEditor.getRootComponent());
		
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
