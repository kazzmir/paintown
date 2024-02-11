package com.rafkind.paintown.animator.events;

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import com.rafkind.paintown.animator.DrawArea;
import com.rafkind.paintown.animator.Animation;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.animator.events.AnimationEvent;
import org.swixml.SwingEngine;

import com.rafkind.paintown.animator.events.scala.AnimationEventNotifier;

public class TrailEvent extends AnimationEventNotifier implements AnimationEvent {
    private int generate;
    private int length;

    public void loadToken(Token token){
        Token token_generate = token.findToken("generate");
        if (token_generate != null){
            generate = token_generate.readInt(0);
        }
        Token token_length = token.findToken("length");
        if (token_length != null){
            length = token_length.readInt(0);
        }
    }

    public AnimationEvent copy(){
        TrailEvent event = new TrailEvent();
        event.generate = generate;
        event.length = length;
        return event;
    }

    public void interact(Animation animation){
    }

    public String getName(){
        return getToken().toString();
    }

    public JPanel getEditor(final Animation animation, final DrawArea area){
        final SwingEngine engine = new SwingEngine("animator/event-trail.xml");
        ((JPanel)engine.getRootComponent()).setSize(200,150);

        final JSpinner spin_generate = (JSpinner) engine.find("generate");
        spin_generate.setValue(new Integer(generate));
        spin_generate.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent changeEvent){
                generate = ((Integer) spin_generate.getValue()).intValue();
                interact(animation);
                updateListeners();
                /* redraw when trails are working in the editor */
                // animation.forceRedraw();
            }
        });

        final JSpinner spin_length = (JSpinner) engine.find("length");
        spin_length.setValue(new Integer(length));
        spin_length.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent changeEvent){
                length = ((Integer) spin_length.getValue()).intValue();
                interact(animation);
                updateListeners();
                // animation.forceRedraw();
            }
        });

        return (JPanel)engine.getRootComponent();
    }

    public Token getToken(){
        Token temp = new Token("trail");
        temp.addToken(new Token("trail"));
        temp.addToken(new String[]{"generate", String.valueOf(generate)});
        temp.addToken(new String[]{"length", String.valueOf(length)});
        return temp;
    }
        
    public void destroy(){
    }

    public String getDescription(){
        return "Creates a fading shadow that trails the player while they move. The 'generate' property sets the amount of time to wait before a new shadow is created. The 'length' property sets how long a shadow lives for.";
    }
}
