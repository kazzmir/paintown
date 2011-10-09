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

public class NopEvent implements AnimationEvent{	
	public void loadToken(Token token){
		// Nothing to be done
	}
	
	public void interact( Animation animation ){
		animation.delay();
	}

    public AnimationEvent copy(){
        return new NopEvent();
    }
	
	public String getName(){
		return getToken().toString();
	}
	
	public JPanel getEditor(final Animation animation, final DrawArea area){
		// Not necessary
		return null;
	}
	
	public Token getToken(){
		Token temp = new Token("nop");
		temp.addToken(new Token("nop"));
		return temp;
	}

        public void destroy(){
        }
}
