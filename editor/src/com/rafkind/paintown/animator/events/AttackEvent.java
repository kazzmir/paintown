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

public class AttackEvent implements AnimationEvent
{
	private int _x1;
	private int _y1;
	private int _x2;
	private int _y2;
	private int _damage;
	private int _force;
	
	public void attack(int x1, int y1, int x2, int y2, int force, int damage)
	{
		_x1 = x1;
		_y1 = y1;
		_x2 = x2;
		_y2 = y2;
		_force = force;
		_damage = damage;
	}
	
	public void interact(DrawArea area)
	{
		//area.setImageX(_x);
		//area.setImageY(_y);
	}
	
	public String getName()
	{
		return getToken().toString();
	}
	
	public JDialog getEditor()
	{
		SwingEngine engine = new SwingEngine( "animator/eventattack.xml" );
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
		final JSpinner forcespin = (JSpinner) engine.find( "force" );
		forcespin.setValue(new Integer(_force));
		forcespin.addChangeListener( new ChangeListener()
		{
			public void stateChanged(ChangeEvent changeEvent)
			{
				_force = ((Integer)forcespin.getValue()).intValue();
			}
		});
		final JSpinner damagespin = (JSpinner) engine.find( "damage" );
		damagespin.setValue(new Integer(_damage));
		damagespin.addChangeListener( new ChangeListener()
		{
			public void stateChanged(ChangeEvent changeEvent)
			{
				_damage = ((Integer)damagespin.getValue()).intValue();
			}
		});
		
		
		return (JDialog)engine.getRootComponent();
	}
	
	public Token getToken()
	{
		Token temp = new Token("attack");
		if(_x1 == 0 && _y1 == 0 && _x2 == 0 && _y2 == 0 && _force == 0 && _damage == 0)
		{
			temp.addToken(new Token("attack"));
		}
		else
		{
			temp.addToken(new Token("attack"));
			temp.addToken(new String[]{"x1",Integer.toString(_x1)});
			temp.addToken(new String[]{"y1",Integer.toString(_y1)});
			temp.addToken(new String[]{"x2",Integer.toString(_x2)});
			temp.addToken(new String[]{"y2",Integer.toString(_y2)});
			temp.addToken(new String[]{"force",Integer.toString(_force)});
			temp.addToken(new String[]{"damage",Integer.toString(_damage)});
		}
		return temp;
	}
}
