package com.rafkind.paintown.animator.events;

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import com.rafkind.paintown.RelativeFileChooser;
import com.rafkind.paintown.animator.NewAnimator;
import com.rafkind.paintown.animator.DrawArea;
import com.rafkind.paintown.animator.Animation;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.animator.events.AnimationEvent;
import org.swixml.SwingEngine;

import com.rafkind.paintown.animator.events.scala.AnimationEventNotifier;

public class TicketEvent extends AnimationEventNotifier implements AnimationEvent {
	
	public void loadToken(Token token){
	}
	
	public void interact( Animation animation ){
	}

    public AnimationEvent copy(){
        return new TicketEvent();
    }
	
	public String getName(){
		return getToken().toString();
	}
	
	public JPanel getEditor(final Animation animation, final DrawArea area){
		JPanel j = new JPanel();
		j.setSize( 200, 50 );
		return j;
		/*
		final SwingEngine engine = new SwingEngine( "animator/eventsound.xml" );
		((JDialog)engine.getRootComponent()).setSize(200,50);
		return (JDialog)engine.getRootComponent();
		*/
	}
	
	public Token getToken(){
		Token temp = new Token("next-ticket");
		temp.addToken(new Token("next-ticket"));
		return temp;
	}
        
    public void destroy(){
    }

    public String getDescription(){
        return "Resets the character's attack state so that they can hit the same enemy while in the same animation. Normally when a character collides with an enemy the character is not allowed hit that same enemy while the character is in the same animation. Use 'next-ticket' if you want an attack to hit multiple times.";
    }
}
