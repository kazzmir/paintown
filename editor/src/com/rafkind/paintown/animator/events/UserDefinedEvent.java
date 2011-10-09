package com.rafkind.paintown.animator.events;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import org.swixml.SwingEngine;

import com.rafkind.paintown.animator.Animation;
import com.rafkind.paintown.animator.DrawArea;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.animator.events.AnimationEvent;

public class UserDefinedEvent implements AnimationEvent {
    private String name;
    private String value;

    public UserDefinedEvent(){
        name = "";
        value = "";
    }

    public AnimationEvent copy(){
        UserDefinedEvent event = new UserDefinedEvent();

        event.name = name;
        event.value = value;

        return event;
    }
    
    public void loadToken(Token token){
		name = token.readString(0);
		value = token.readString(1);
    }

    private String maybeString(String value){
        if (value.indexOf(" ") != -1){
            return "\"" + value + "\"";
        }
        return value;
    }
    
    public Token getToken(){
        Token temp = new Token("user");
		temp.addToken(new Token("user"));
		temp.addToken(new Token(maybeString(name)));
		temp.addToken(new Token(maybeString(value)));
		
		return temp;
    }
    
    public void interact(Animation animation){
    }
    
    public String getName(){
        return getToken().toString();
    }

    public JPanel getEditor(final Animation animation, final DrawArea area){

        SwingEngine engine = new SwingEngine( "animator/event-user.xml" );
		final JTextField namePanel = (JTextField) engine.find("name");
		final JTextField valuePanel = (JTextField) engine.find("value");

        namePanel.setText(name);
        valuePanel.setText(value);
        namePanel.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent e){
                name = namePanel.getText();
            }
        });

        valuePanel.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent e){
                value = valuePanel.getText();
            }
        });

        return (JPanel) engine.getRootComponent();

    }

    public void destroy(){
    }
}
