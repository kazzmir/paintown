package com.rafkind.paintown.animator.events;

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import com.rafkind.paintown.animator.Animation;
import com.rafkind.paintown.animator.DrawArea;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.animator.events.AnimationEvent;
import org.swixml.SwingEngine;

import com.rafkind.paintown.animator.events.scala.AnimationEventNotifier;

public class CoordsEvent extends AnimationEventNotifier implements AnimationEvent
{
	private int _x;
	private int _y;
	private int _z;
	
	public void loadToken(Token token)
	{
		_x = token.readInt(0);
		_y = token.readInt(1);
		_z = token.readInt(2);
	}
	
	public void interact( Animation animation ){
		
	}

    public AnimationEvent copy(){
        return null;
    }
	
	public String getName()
	{
		return getToken().toString();
	}
	
	public JPanel getEditor(final Animation animation, final DrawArea area){
		SwingEngine engine = new SwingEngine( "animator/eventcoords.xml" );
		((JPanel)engine.getRootComponent()).setSize(200,100);
		
		final JSpinner xspin = (JSpinner) engine.find( "x" );
		xspin.setValue(new Integer(_x));
		xspin.addChangeListener( new ChangeListener(){
			public void stateChanged(ChangeEvent changeEvent){
				_x = ((Integer)xspin.getValue()).intValue();
                updateListeners();
			}
		});
		final JSpinner yspin = (JSpinner) engine.find( "y" );
		yspin.setValue(new Integer(_y));
		yspin.addChangeListener( new ChangeListener()
		{
			public void stateChanged(ChangeEvent changeEvent)
			{
				_y = ((Integer)yspin.getValue()).intValue();
                updateListeners();
			}
		});
		final JSpinner zspin = (JSpinner) engine.find( "z" );
		zspin.setValue(new Integer(_z));
		zspin.addChangeListener( new ChangeListener()
		{
			public void stateChanged(ChangeEvent changeEvent)
			{
				_z = ((Integer)zspin.getValue()).intValue();
                updateListeners();
			}
		});
		return (JPanel)engine.getRootComponent();
	}
	
	public Token getToken()
	{
		Token temp = new Token("coords");
		temp.addToken(new Token("coords"));
		temp.addToken(new Token(Integer.toString(_x)));
		temp.addToken(new Token(Integer.toString(_y)));
		temp.addToken(new Token(Integer.toString(_z)));
		
		return temp;
	}
        
    public void destroy(){
    }

    public String getDescription(){
        return "Not used";
    }
}
