package com.rafkind.paintown.animator.events;

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import com.rafkind.paintown.animator.DrawArea;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.animator.events.AnimationEvent;
import org.swixml.SwingEngine;

public class BBoxEvent implements AnimationEvent
{
	private int _x1;
	private int _y1;
	private int _x2;
	private int _y2;
	
	public void loadToken(Token token)
	{
		_x1 = token.readInt(0);
		_y1 = token.readInt(1);
		_x2 = token.readInt(2);
		_y2 = token.readInt(3);
	}
	
	public void interact(DrawArea area)
	{
	 			 //area.setAttack(new BoundingBox(_x1,_y1,_x2,_y2));
	}
	
	public String getName()
	{
		return getToken().toString();
	}
	
	public JDialog getEditor()
	{
		SwingEngine engine = new SwingEngine( "animator/eventbbox.xml" );
		((JDialog)engine.getRootComponent()).setSize(200,150);
		
		final JSpinner x1spin = (JSpinner) engine.find( "x1" );
		x1spin.setValue(new Integer(_x1));
		x1spin.addChangeListener( new ChangeListener()
		{
			public void stateChanged(ChangeEvent changeEvent)
			{
				_x1 = ((Integer)x1spin.getValue()).intValue();
			}
		});
		final JSpinner y1spin = (JSpinner) engine.find( "y1" );
		y1spin.setValue(new Integer(_y1));
		y1spin.addChangeListener( new ChangeListener()
		{
			public void stateChanged(ChangeEvent changeEvent)
			{
				_y1 = ((Integer)y1spin.getValue()).intValue();
			}
		});
		final JSpinner x2spin = (JSpinner) engine.find( "x2" );
		x2spin.setValue(new Integer(_x2));
		x2spin.addChangeListener( new ChangeListener()
		{
			public void stateChanged(ChangeEvent changeEvent)
			{
				_x2 = ((Integer)x2spin.getValue()).intValue();
			}
		});
		final JSpinner y2spin = (JSpinner) engine.find( "y2" );
		y2spin.setValue(new Integer(_y2));
		y2spin.addChangeListener( new ChangeListener()
		{
			public void stateChanged(ChangeEvent changeEvent)
			{
				_y2 = ((Integer)y2spin.getValue()).intValue();
			}
		});
		
		return (JDialog)engine.getRootComponent();
	}
	
	public Token getToken()
	{
		Token temp = new Token("bbox");
		temp.addToken(new Token("bbox"));
		temp.addToken(new Token(Integer.toString(_x1)));
		temp.addToken(new Token(Integer.toString(_y1)));
		temp.addToken(new Token(Integer.toString(_x2)));
		temp.addToken(new Token(Integer.toString(_y2)));
		
		return temp;
	}
}
