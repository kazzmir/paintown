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

public class StatusEvent extends AnimationEventNotifier implements AnimationEvent {
	private String _status = "ground";
	
	public void loadToken(Token token){
		_status = token.readString(0);
	}
	
	public void interact( Animation animation ){
	}

    public AnimationEvent copy(){
        StatusEvent event = new StatusEvent();
        event._status = _status;
        return event;
    }
	
	public String getName(){
		return getToken().toString();
	}
	
	public JPanel getEditor(final Animation animation, final DrawArea area){
		SwingEngine engine = new SwingEngine( "animator/eventstatus.xml" );
		((JPanel)engine.getRootComponent()).setSize(200,50);
		
		final JComboBox statusbox = (JComboBox) engine.find( "status" );
		statusbox.addItem(new String("ground"));
		statusbox.addItem(new String("jump"));
		statusbox.addItem(new String("grab"));
		
		statusbox.addActionListener( new ActionListener()
		{
			public void actionPerformed(ActionEvent actionEvent)
			{
				_status = (String)statusbox.getSelectedItem();
                updateListeners();
			}
		});
		return (JPanel)engine.getRootComponent();
	}
	
	public Token getToken()
	{
		Token temp = new Token("status");
		temp.addToken(new Token("status"));
		temp.addToken(new Token(_status));
		
		return temp;
	}
        
    public void destroy(){
    }

    public String getDescription(){
        return "Not used";
    }
}
