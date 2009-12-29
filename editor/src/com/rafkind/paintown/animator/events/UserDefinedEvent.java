package com.rafkind.paintown.animator.events;

import javax.swing.*;
import javax.swing.event.*;

import com.rafkind.paintown.animator.Animation;
import com.rafkind.paintown.animator.DrawArea;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.animator.events.AnimationEvent;

public class UserDefinedEvent implements AnimationEvent {
    private String name;
    private String value;

    public UserDefinedEvent(){
    }
    
    public void loadToken(Token token){
		name = token.readString(0);
		value = token.readString(1);
    }
    
    public Token getToken(){
        Token temp = new Token("user");
		temp.addToken(new Token("user"));
		temp.addToken(new Token(name));
		temp.addToken(new Token(value));
		
		return temp;
    }
    
    public void interact(Animation animation){
        /* nothing */
    }
    
    public String getName(){
        return getToken().toString();
    }

    public JPanel getEditor(final Animation animation, final DrawArea area){
        return null;
    }

    public void destroy(){
    }
}
