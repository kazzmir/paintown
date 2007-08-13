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

public class ZDistanceEvent implements AnimationEvent
{
	private int _d;
	
	public void loadToken(Token token)
	{
		_d = token.readInt(0);
	}
	
	public void interact(DrawArea area)
	{
		
	}
	
	public String getName()
	{
		return getToken().toString();
	}
	
	public JDialog getEditor()
	{
		SwingEngine engine = new SwingEngine( "animator/eventzdistance.xml" );
		((JDialog)engine.getRootComponent()).setSize(200,50);
		
		final JSpinner dspin = (JSpinner) engine.find( "zdistance" );
		dspin.setValue(new Integer(_d));
		dspin.addChangeListener( new ChangeListener()
		{
			public void stateChanged(ChangeEvent changeEvent)
			{
				_d = ((Integer)dspin.getValue()).intValue();
			}
		});
		return (JDialog)engine.getRootComponent();
	}
	
	public Token getToken()
	{
		Token temp = new Token("z-distance");
		temp.addToken(new Token("z-distance"));
		temp.addToken(new Token(Integer.toString(_d)));
		
		return temp;
	}
}
