package com.rafkind.paintown.animator.events;

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import com.rafkind.paintown.RelativeFileChooser;
import com.rafkind.paintown.animator.Animator;
import com.rafkind.paintown.animator.DrawArea;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.animator.events.AnimationEvent;
import org.swixml.SwingEngine;

public class SoundEvent implements AnimationEvent
{
	private String _sound;
	
	public void loadToken(Token token)
	{
		Token soundToken = token.findToken( "sound" );
		if(soundToken != null)
		{
			_sound = soundToken.readString(0);
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
		final SwingEngine engine = new SwingEngine( "animator/eventsound.xml" );
		((JDialog)engine.getRootComponent()).setSize(200,50);
		
		final JTextField soundfield = (JTextField) engine.find( "sound" );
		final JButton soundbutton = (JButton) engine.find( "sound-button" );
		soundfield.setText(_sound);
		soundbutton.addActionListener( new AbstractAction(){
				public void actionPerformed( ActionEvent event ){
					RelativeFileChooser chooser = Animator.getNewFileChooser();
					int ret = chooser.open();
					if ( ret == RelativeFileChooser.OK ){
						final String path = chooser.getPath();
						soundfield.setText( path );
						_sound = path;
					}
				}
			});
			
		return (JDialog)engine.getRootComponent();
	}
	
	public Token getToken()
	{
		Token temp = new Token("sound");
		temp.addToken(new Token("sound"));
		temp.addToken(new Token(_sound));
		
		return temp;
	}
}
