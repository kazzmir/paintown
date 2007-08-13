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

public class FaceEvent implements AnimationEvent
{
	private String _face = "reverse";
	
	public void face(String face)
	{
		_face = face;
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
		SwingEngine engine = new SwingEngine( "animator/eventface.xml" );
		((JDialog)engine.getRootComponent()).setSize(200,50);
		
		final JComboBox facebox = (JComboBox) engine.find( "face" );
		facebox.addItem(new String("reverse"));
		
		facebox.addActionListener( new ActionListener()
		{
			public void actionPerformed(ActionEvent actionEvent)
			{
				_face = (String)facebox.getSelectedItem();
			}
		});
		return (JDialog)engine.getRootComponent();
	}
	
	public Token getToken()
	{
		Token temp = new Token("face");
		temp.addToken(new Token("face"));
		temp.addToken(new Token(_face));
		
		return temp;
	}
}
