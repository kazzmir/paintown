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

public class DelayEvent implements AnimationEvent {
    private int _delay;

    public void loadToken(Token token){
        _delay = token.readInt(0);
    }

    public void interact( Animation animation ){
        animation.setDelay( _delay );
    }

    public String getName(){
        return getToken().toString();
    }

    public JPanel getEditor( Animation animation ){
        SwingEngine engine = new SwingEngine( "animator/eventdelay.xml" );
        ((JPanel)engine.getRootComponent()).setSize(200,100);

        final JSpinner delayspin = (JSpinner) engine.find( "delay" );
        delayspin.setValue(new Integer(_delay));
        delayspin.addChangeListener( new ChangeListener(){
            public void stateChanged(ChangeEvent changeEvent){
                _delay = ((Integer)delayspin.getValue()).intValue();
            }
        });
        return (JPanel)engine.getRootComponent();
    }

    public Token getToken(){
        Token temp = new Token("delay");
        temp.addToken(new Token("delay"));
        temp.addToken(new Token(Integer.toString(_delay)));
        return temp;
    }
}
