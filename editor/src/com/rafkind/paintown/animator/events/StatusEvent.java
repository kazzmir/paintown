package com.rafkind.paintown.animator.events;

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import com.rafkind.paintown.animator.Animation;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.animator.events.AnimationEvent;
import org.swixml.SwingEngine;

public class StatusEvent implements AnimationEvent {
	private String _status = "ground";
	
	public void loadToken(Token token)
	{
		_status = token.readString(0);
	}
	
	public void interact( Animation animation ){
		
	}
	
	public String getName(){
		return getToken().toString();
	}
	
	public JDialog getEditor()
	{
		SwingEngine engine = new SwingEngine( "animator/eventstatus.xml" );
		((JDialog)engine.getRootComponent()).setSize(200,50);
		
		final JComboBox statusbox = (JComboBox) engine.find( "status" );
		statusbox.addItem(new String("ground"));
		statusbox.addItem(new String("jump"));
		
		statusbox.addActionListener( new ActionListener()
		{
			public void actionPerformed(ActionEvent actionEvent)
			{
				_status = (String)statusbox.getSelectedItem();
			}
		});
		return (JDialog)engine.getRootComponent();
	}
	
	public Token getToken()
	{
		Token temp = new Token("status");
		temp.addToken(new Token("status"));
		temp.addToken(new Token(_status));
		
		return temp;
	}
}
