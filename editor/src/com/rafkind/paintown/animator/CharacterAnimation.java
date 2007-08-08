package com.rafkind.paintown.animator;

import java.util.*;
import javax.swing.*;
import java.awt.*;
import javax.swing.event.*;

import org.swixml.SwingEngine;

import com.rafkind.paintown.Token;
import com.rafkind.paintown.animator.DrawArea;
import com.rafkind.paintown.animator.SpecialPanel;

public class CharacterAnimation
{
	private DrawArea area;
	private SwingEngine animEditor;
	private SwingEngine contextEditor;
	private JPanel context;
	private JPanel canvas;
	private JPanel other;
	private JTextField nameField;
	private JComboBox typeCombo;
	private JSpinner rangeSpinner;
	private JComboBox faceCombo;
	private JTextField basedirField;
	private JButton basedirButton;
	
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
	
	public SpecialPanel getEditor()
	{	
		return new SpecialPanel((JPanel)animEditor.getRootComponent(),nameField);
	}
	
	public CharacterAnimation()
	{
		name = "New Animation";
		
		animEditor = new SwingEngine( "animator/base.xml" );
		
		contextEditor = new SwingEngine ( "animator/animation.xml");
		
		context = (JPanel) animEditor.find( "context" );
		
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
		
		typeCombo = (JComboBox) contextEditor.find( "type" );
		rangeSpinner = (JSpinner) contextEditor.find( "range" );
		faceCombo = (JComboBox) contextEditor.find( "face" );
		basedirField = (JTextField) contextEditor.find( "basedir" );
		basedirButton = (JButton) contextEditor.find( "change-basedir" );
		
		canvas = (JPanel) animEditor.find( "canvas" );
		// area = new DrawArea();
		// canvas.add(area);
		
		other = (JPanel) animEditor.find( "other" );
		
		context.add((JComponent)contextEditor.getRootComponent());
	}
}
