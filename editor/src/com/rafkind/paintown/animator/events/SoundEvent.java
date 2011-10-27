package com.rafkind.paintown.animator.events;

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import com.rafkind.paintown.RelativeFileChooser;
import com.rafkind.paintown.animator.DrawArea;
import com.rafkind.paintown.animator.NewAnimator;
import com.rafkind.paintown.animator.Animation;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.animator.events.AnimationEvent;
import org.swixml.SwingEngine;

public class SoundEvent implements AnimationEvent
{
	private String _sound;
	
	public void loadToken(Token token){
		_sound = token.readString(0);
	}
	
	public void interact( Animation animation ){
	}

    public AnimationEvent copy(){
        SoundEvent event = new SoundEvent();

        event._sound = _sound;

        return event;
    }
	
	public String getName(){
		return getToken().toString();
	}
	
	public JPanel getEditor(final Animation animation, final DrawArea area){
		final SwingEngine engine = new SwingEngine( "animator/eventsound.xml" );
		((JPanel)engine.getRootComponent()).setSize(200,50);
		
		final JTextField soundfield = (JTextField) engine.find( "sound" );
		final JButton soundbutton = (JButton) engine.find( "sound-button" );
		soundfield.setText(_sound);
		soundbutton.addActionListener( new AbstractAction(){
				public void actionPerformed( ActionEvent event ){
					RelativeFileChooser chooser = NewAnimator.getNewFileChooser();
					int ret = chooser.open();
					if ( ret == RelativeFileChooser.OK ){
						final String path = chooser.getPath();
						soundfield.setText( path );
						_sound = path;
					}
				}
			});
			
		return (JPanel)engine.getRootComponent();
	}
	
	public Token getToken()
	{
		Token temp = new Token("sound");
		temp.addToken(new Token("sound"));
		temp.addToken(new Token(_sound));
		
		return temp;
	}
        
    public void destroy(){
    }

    public String getDescription(){
        return "Plays a sound from a given file.";
    }
}
