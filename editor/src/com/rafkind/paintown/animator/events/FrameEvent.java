package com.rafkind.paintown.animator.events;

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import com.rafkind.paintown.animator.Animator;
import com.rafkind.paintown.animator.DrawArea;
import com.rafkind.paintown.animator.DrawState;
import com.rafkind.paintown.MaskedImage;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.animator.events.AnimationEvent;
import org.swixml.SwingEngine;

public class FrameEvent implements AnimationEvent
{
	private String _frame = "";
	
	public void loadToken(Token token)
	{
		Token parent = token.getParent();
		Token basedir = parent.findToken("basedir");
		if(basedir != null)
		{
			_frame = basedir.readString(0) + token.readString(0);
		}
		else _frame = token.readString(0);
	}
	
	public void interact(DrawArea area)
	{
		try
		{
			area.setImage(MaskedImage.load(Animator.getDataPath() + "/" + _frame));
		}
		catch(Exception e)
		{
			System.out.println("Problem! --> " + Animator.getDataPath() + "/" + _frame);
		}
	}
	
	public String getName()
	{
		return getToken().toString();
	}
	
	public JDialog getEditor()
	{
		SwingEngine engine = new SwingEngine( "animator/eventframe.xml" );
		((JDialog)engine.getRootComponent()).setSize(200,50);
		
		final JComboBox framebox = (JComboBox) engine.find( "frame" );
		Iterator eItor = DrawState.getCurrentDirList().iterator();
		while(eItor.hasNext())
		{
			String frame = (String)eItor.next();
			framebox.addItem(frame);
		}
		
		framebox.addActionListener( new ActionListener()
		{
			public void actionPerformed(ActionEvent actionEvent)
			{
				_frame = (String)framebox.getSelectedItem();
			}
		});
		return (JDialog)engine.getRootComponent();
	}
	
	public Token getToken()
	{
		Token temp = new Token("frame");
		temp.addToken(new Token("frame"));
		temp.addToken(new Token(_frame));
		
		return temp;
	}
}
