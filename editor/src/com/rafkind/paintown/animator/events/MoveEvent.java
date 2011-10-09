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

public class MoveEvent implements AnimationEvent {
	private int _x;
	private int _y;
	private int _z;
	
	public void loadToken(Token token) {
		_x = token.readInt(0);
		_y = token.readInt(1);
		_z = token.readInt(2);
	}

    public AnimationEvent copy(){
        MoveEvent event = new MoveEvent();
        event._x = _x;
        event._y = _y;
        event._z = _z;
        return event;
    }
	
	public void interact( Animation animation ){
		animation.moveX( _x );
		animation.moveY( _y );
	}
	
	public String getName()
	{
		return getToken().toString();
	}
	
	public JPanel getEditor(final Animation animation, final DrawArea area){
		SwingEngine engine = new SwingEngine( "animator/eventmove.xml" );
		((JPanel)engine.getRootComponent()).setSize(200,150);
		
		final JSpinner xspin = (JSpinner) engine.find( "x" );
		xspin.setValue(new Integer(_x));
		xspin.addChangeListener( new ChangeListener(){
			public void stateChanged(ChangeEvent changeEvent){
				_x = ((Integer)xspin.getValue()).intValue();
			}
		});
		final JSpinner yspin = (JSpinner) engine.find( "y" );
		yspin.setValue(new Integer(_y));
		yspin.addChangeListener( new ChangeListener(){
			public void stateChanged(ChangeEvent changeEvent){
				_y = ((Integer)yspin.getValue()).intValue();
			}
		});
		final JSpinner zspin = (JSpinner) engine.find( "z" );
		zspin.setValue(new Integer(_z));
		zspin.addChangeListener( new ChangeListener(){
			public void stateChanged(ChangeEvent changeEvent){
				_z = ((Integer)zspin.getValue()).intValue();
			}
		});
		return (JPanel)engine.getRootComponent();
	}
	
	public Token getToken()
	{
		Token temp = new Token("move");
		temp.addToken(new Token("move"));
		temp.addToken(new Token(Integer.toString(_x)));
		temp.addToken(new Token(Integer.toString(_y)));
		temp.addToken(new Token(Integer.toString(_z)));
		
		return temp;
	}

        public void destroy(){
        }
}
