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

public class DelayEvent implements AnimationEvent {
    private double _delay;

    public void loadToken(Token token){
        _delay = token.readDouble(0);
    }

    public void interact( Animation animation ){
        animation.setDelay( _delay );
    }

    public String getName(){
        return getToken().toString();
    }

    public AnimationEvent copy(){
        DelayEvent event = new DelayEvent();
        event._delay = _delay;
        return event;
    }

    public JPanel getEditor(final Animation animation, final DrawArea area){
        SwingEngine engine = new SwingEngine("animator/eventdelay.xml");
        ((JPanel)engine.getRootComponent()).setSize(200,100);

        final JSpinner delayspin = (JSpinner) engine.find("delay");
        delayspin.setModel(new SpinnerNumberModel(_delay, 0, 99999, 1));
        delayspin.addChangeListener(new ChangeListener(){
            public void stateChanged(ChangeEvent changeEvent){
                _delay = ((Number) delayspin.getValue()).doubleValue();
            }
        });
        return (JPanel)engine.getRootComponent();
    }

    public Token getToken(){
        Token temp = new Token("delay");
        temp.addToken(new Token("delay"));
        temp.addToken(new Token(Double.toString(_delay)));
        return temp;
    }

    public void destroy(){
    }

    public String getDescription(){
        return "Sets the amount of time (given in terms of ticks of the game) to wait before moving on to the next event. Only certain events wait for the delay time such as 'frame' and 'nop'.";
    }
}
