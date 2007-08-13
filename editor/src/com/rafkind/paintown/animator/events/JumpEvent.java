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

public class JumpEvent implements AnimationEvent
{
	private double _x;
	private double _y;
	private double _z;
	
	public void loadToken(Token token)
	{
		Token jumpToken = token.findToken( "jump" );
		if(jumpToken != null)
		{
			_x = jumpToken.readDouble(0);
			_y = jumpToken.readDouble(1);
			_z = jumpToken.readDouble(2);
		}
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
		SwingEngine engine = new SwingEngine( "animator/eventjump.xml" );
		((JDialog)engine.getRootComponent()).setSize(200,150);
		
		final JPanel xpanel = (JPanel) engine.find( "x" );
		final JSpinner xspin = new JSpinner(new SpinnerNumberModel(0, -1000, 1000, .01));
		xpanel.add(xspin);
		xspin.setValue(new Double(_x));
		xspin.addChangeListener( new ChangeListener()
		{
			public void stateChanged(ChangeEvent changeEvent)
			{
				_x = ((Double)xspin.getValue()).doubleValue();
			}
		});
		final JPanel ypanel = (JPanel) engine.find( "y" );
		final JSpinner yspin = new JSpinner(new SpinnerNumberModel(0, -1000, 1000, .01));
		ypanel.add(yspin);
		yspin.setValue(new Double(_y));
		yspin.addChangeListener( new ChangeListener()
		{
			public void stateChanged(ChangeEvent changeEvent)
			{
				_y = ((Double)yspin.getValue()).doubleValue();
			}
		});
		final JPanel zpanel = (JPanel) engine.find( "z" );
		final JSpinner zspin = new JSpinner(new SpinnerNumberModel(0, -1000, 1000, .01));
		zpanel.add(zspin);
		zspin.setValue(new Double(_z));
		zspin.addChangeListener( new ChangeListener()
		{
			public void stateChanged(ChangeEvent changeEvent)
			{
				_z = ((Double)zspin.getValue()).doubleValue();
			}
		});
		return (JDialog)engine.getRootComponent();
	}
	
	public Token getToken()
	{
		Token temp = new Token("jump");
		temp.addToken(new Token("jump"));
		temp.addToken(new Token(Double.toString(_x)));
		temp.addToken(new Token(Double.toString(_y)));
		temp.addToken(new Token(Double.toString(_z)));
		
		return temp;
	}
}
