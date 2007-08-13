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

public class DelayEvent implements AnimationEvent
{
	private int _delay;
	
	public void loadToken(Token token)
	{
		_delay = token.readInt(0);
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
		SwingEngine engine = new SwingEngine( "animator/eventdelay.xml" );
		((JDialog)engine.getRootComponent()).setSize(200,50);
		
		final JSpinner delayspin = (JSpinner) engine.find( "delay" );
		delayspin.setValue(new Integer(_delay));
		delayspin.addChangeListener( new ChangeListener()
		{
			public void stateChanged(ChangeEvent changeEvent)
			{
				_delay = ((Integer)delayspin.getValue()).intValue();
			}
		});
		return (JDialog)engine.getRootComponent();
	}
	
	public Token getToken()
	{
		Token temp = new Token("delay");
		temp.addToken(new Token("delay"));
		temp.addToken(new Token(Integer.toString(_delay)));
		
		return temp;
	}
}
