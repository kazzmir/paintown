package com.rafkind.paintown.animator.events;

import javax.swing.*;
import javax.swing.event.*;

import com.rafkind.paintown.animator.Animation;
import com.rafkind.paintown.animator.DrawArea;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.animator.events.AnimationEvent;

public class UserDefinedEvent implements AnimationEvent {
    public UserDefinedEvent(){
    }
    
    public void loadToken(Token token){
    }
    
    public Token getToken(){
        return null;
    }
    
    public void interact(Animation animation){
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
