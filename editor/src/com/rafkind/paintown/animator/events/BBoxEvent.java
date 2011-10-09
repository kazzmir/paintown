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

public class BBoxEvent implements AnimationEvent {
    private class Box{
        public Box(int x1, int y1, int x2, int y2){
            this.x1 = x1;
            this.x2 = x2;
            this.y1 = y1;
            this.y2 = y2;
        }

        public Box(){
            this(0,0,0,0);
        }

        public int x1;
        public int y1;
        public int x2;
        public int y2;
    }
    
    private Box box = new Box();

    public void loadToken(Token token){
        box.x1 = token.readInt(0);
        box.y1 = token.readInt(1);
        box.x2 = token.readInt(2);
        box.y2 = token.readInt(3);
    }

    public void interact(Animation animation){
        //area.setAttack(new BoundingBox(_x1,_y1,_x2,_y2));
    }

    public AnimationEvent copy(){
        return null;
    }

    public String getName(){
        return getToken().toString();
    }

    public JPanel getEditor(final Animation animation, final DrawArea area){
        SwingEngine engine = new SwingEngine( "animator/eventbbox.xml" );
        ((JPanel)engine.getRootComponent()).setSize(200,150);

        final JSpinner x1spin = (JSpinner) engine.find( "x1" );
        x1spin.setValue(new Integer(box.x1));
        x1spin.addChangeListener( new ChangeListener()
                {
                    public void stateChanged(ChangeEvent changeEvent)
        {
            box.x1 = ((Integer)x1spin.getValue()).intValue();
        }
        });
        final JSpinner y1spin = (JSpinner) engine.find( "y1" );
        y1spin.setValue(new Integer(box.y1));
        y1spin.addChangeListener( new ChangeListener()
                {
                    public void stateChanged(ChangeEvent changeEvent)
        {
            box.y1 = ((Integer)y1spin.getValue()).intValue();
        }
        });
        final JSpinner x2spin = (JSpinner) engine.find( "x2" );
        x2spin.setValue(new Integer(box.x2));
        x2spin.addChangeListener( new ChangeListener()
                {
                    public void stateChanged(ChangeEvent changeEvent)
        {
            box.x2 = ((Integer)x2spin.getValue()).intValue();
        }
        });
        final JSpinner y2spin = (JSpinner) engine.find( "y2" );
        y2spin.setValue(new Integer(box.y2));
        y2spin.addChangeListener( new ChangeListener()
                {
                    public void stateChanged(ChangeEvent changeEvent)
        {
            box.y2 = ((Integer)y2spin.getValue()).intValue();
        }
        });

        return (JPanel)engine.getRootComponent();
    }

    public Token getToken(){
        Token temp = new Token("bbox");
        temp.addToken(new Token("bbox"));
        temp.addToken(new Token(Integer.toString(box.x1)));
        temp.addToken(new Token(Integer.toString(box.y1)));
        temp.addToken(new Token(Integer.toString(box.x2)));
        temp.addToken(new Token(Integer.toString(box.y2)));

        return temp;
    }

    public void destroy(){
    }
}
