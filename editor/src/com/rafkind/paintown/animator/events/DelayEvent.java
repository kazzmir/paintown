package com.rafkind.paintown.animator.events;

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import com.rafkind.paintown.animator.Animation;
import com.rafkind.paintown.animator.NewAnimator;
import com.rafkind.paintown.animator.DrawArea;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.animator.events.AnimationEvent;
import org.swixml.SwingEngine;

import com.rafkind.paintown.animator.events.scala.AnimationEventNotifier;

public class DelayEvent extends AnimationEventNotifier implements AnimationEvent {
    /* Measure time in either game ticks (Ticks), milliseconds, or seconds */
    private final static int Ticks = 0;
    private final static int Milliseconds = 1;
    private final static int Seconds = 2;

    private double delay;
    private int delayType = Ticks;

    public void loadToken(Token token){
        delay = token.readDouble(0);
        if (token.hasIndex(1)){
            String type = token.readString(1);
            delayType = parseType(type.toLowerCase());
        }
    }

    private int parseType(String type){
        if (type == "ticks"){
            return Ticks;
        } else if (type == "ms" || type == "milliseconds"){
            return Milliseconds;
        } else if (type == "s" || type == "sec" || type == "seconds"){
            return Seconds;
        }
        return Ticks;
    }

    private String canonicalType(){
        switch (delayType){
            case Ticks: return "ticks";
            case Milliseconds: return "ms";
            case Seconds: return "s";
        }
        return "ticks";
    }

    /* Delay in milliseconds */
    private double delayAmount(){
        switch (delayType){
            case Ticks: return 1000 * delay / NewAnimator.getTicksPerSecond();
            case Milliseconds: return delay;
            case Seconds: return delay * 1000;
        }
        return 0;
    }

    public void interact(Animation animation){
        animation.setDelay(delayAmount());
    }

    public String getName(){
        return getToken().toString();
    }

    public AnimationEvent copy(){
        DelayEvent event = new DelayEvent();
        event.delay = delay;
        event.delayType = delayType;
        return event;
    }

    public JPanel getEditor(final Animation animation, final DrawArea area){
        SwingEngine engine = new SwingEngine("animator/eventdelay.xml");
        ((JPanel)engine.getRootComponent()).setSize(200,100);

        final JSpinner delayspin = (JSpinner) engine.find("delay");
        delayspin.setModel(new SpinnerNumberModel(delay, 0, 99999, 1));
        delayspin.addChangeListener(new ChangeListener(){
            public void stateChanged(ChangeEvent changeEvent){
                delay = ((Number) delayspin.getValue()).doubleValue();
                updateListeners();
            }
        });

        final JComboBox type = (JComboBox) engine.find("kind");
        type.addItem("ticks");
        type.addItem("ms");
        type.addItem("s");

        switch (delayType){
            case Ticks: type.setSelectedItem("ticks"); break;
            case Milliseconds: type.setSelectedItem("ms"); break;
            case Seconds: type.setSelectedItem("s"); break;
        }

        type.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                String kind = (String) type.getSelectedItem();
                delayType = parseType(kind);
                updateListeners();
            }
        });

        return (JPanel)engine.getRootComponent();
    }

    public Token getToken(){
        Token temp = new Token("delay");
        temp.addToken(new Token("delay"));
        temp.addToken(new Token(Double.toString(delay)));

        /* ticks is default so leave it off */
        if (delayType != Ticks){
            temp.addToken(new Token(canonicalType()));
        }
        return temp;
    }

    public void destroy(){
    }

    public String getDescription(){
        return "Sets the amount of time (given in terms of ticks of the game) to wait before moving on to the next event. Only certain events wait for the delay time such as 'frame' and 'nop'.";
    }
}
