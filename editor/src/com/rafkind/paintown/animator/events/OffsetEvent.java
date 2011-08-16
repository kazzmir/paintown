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

public class OffsetEvent implements AnimationEvent {
	private int _x;
	private int _y;
	
	public void loadToken(Token token){
		_x = token.readInt(0);
		_y = token.readInt(1);
	}
	
	public void interact(Animation animation){
		animation.setOffsetX(_x);
		animation.setOffsetY(_y);
	}

    public void setX(int x){
        _x = x;
    }

    public int getX(){
        return _x;
    }

    public void setY(int y){
        _y = y;
    }

    public int getY(){
        return _y;
    }

	public String getName(){
		return getToken().toString();
	}
	
	public JPanel getEditor(final Animation animation, final DrawArea area){
		SwingEngine engine = new SwingEngine("animator/eventoffset.xml");
		((JPanel)engine.getRootComponent()).setSize(200,100);
		
		final JSpinner xspin = (JSpinner) engine.find("x");
		xspin.setValue(new Integer(_x));
		xspin.addChangeListener(new ChangeListener(){
			public void stateChanged(ChangeEvent changeEvent){
				_x = ((Integer)xspin.getValue()).intValue();
				interact(animation);
				animation.forceRedraw();
			}
		});

		final JSpinner yspin = (JSpinner) engine.find("y");
		yspin.setValue(new Integer(_y));
		yspin.addChangeListener(new ChangeListener(){
			public void stateChanged(ChangeEvent changeEvent){
				_y = ((Integer)yspin.getValue()).intValue();
				interact(animation);
				animation.forceRedraw();
			}
		});
		return (JPanel)engine.getRootComponent();
	}
	
	public Token getToken(){
		Token temp = new Token("offset");
		temp.addToken(new Token("offset"));
		temp.addToken(new Token(String.valueOf(_x)));
		temp.addToken(new Token(String.valueOf(_y)));
		
		return temp;
	}
        
    public void destroy(){
    }
}
