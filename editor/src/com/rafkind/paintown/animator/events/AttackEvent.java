package com.rafkind.paintown.animator.events;

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import com.rafkind.paintown.animator.Animation;
import com.rafkind.paintown.animator.DrawArea;
import com.rafkind.paintown.animator.BoundingBox;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.animator.events.AnimationEvent;
import org.swixml.SwingEngine;

public class AttackEvent implements AnimationEvent {
    private int x1;
    private int y1;
    private int x2;
    private int y2;
    private int damage;
    private int force;

    public void loadToken(Token token){
        Token x1_token = token.findToken("x1");
        if (x1_token != null){
            x1 = x1_token.readInt(0);
        }

        Token y1_token = token.findToken("y1");
        if (y1_token != null){
            y1 = y1_token.readInt(0);
        }

        Token x2_token = token.findToken("x2");
        if (x2_token != null){
            x2 = x2_token.readInt(0);
        }

        Token y2_token = token.findToken("y2");
        if (y2_token != null){
            y2 = y2_token.readInt(0);
        }

        Token damage_token = token.findToken("damage");
        if (damage_token != null){
            damage = damage_token.readInt(0);
        }

        Token force_token = token.findToken("force");
        if (force_token != null){
            force = force_token.readInt(0);
        }
    }

    public void interact(Animation animation){
        animation.setAttack(new BoundingBox(x1,y1,x2,y2));
    }

    public String getName(){
        return getToken().toString();
    }

    public JPanel getEditor(final Animation animation, final DrawArea area){
        SwingEngine engine = new SwingEngine( "animator/eventattack.xml" );
        ((JPanel)engine.getRootComponent()).setSize(200,150);

        final JSpinner x1spin = (JSpinner) engine.find( "x1" );
        x1spin.setValue(new Integer(x1));
        x1spin.addChangeListener( new ChangeListener(){
            public void stateChanged(ChangeEvent changeEvent){
                x1 = ((Integer)x1spin.getValue()).intValue();
                interact(animation);
                animation.forceRedraw();
            }
        });
        final JSpinner y1spin = (JSpinner) engine.find( "y1" );
        y1spin.setValue(new Integer(y1));
        y1spin.addChangeListener( new ChangeListener() {
            public void stateChanged(ChangeEvent changeEvent){
                y1 = ((Integer)y1spin.getValue()).intValue();
                interact( animation );
                animation.forceRedraw();
            }
        });
        final JSpinner x2spin = (JSpinner) engine.find( "x2" );
        x2spin.setValue(new Integer(x2));
        x2spin.addChangeListener( new ChangeListener(){
            public void stateChanged(ChangeEvent changeEvent){
                x2 = ((Integer)x2spin.getValue()).intValue();
                interact( animation );
                animation.forceRedraw();
            }
        });
        final JSpinner y2spin = (JSpinner) engine.find( "y2" );
        y2spin.setValue(new Integer(y2));
        y2spin.addChangeListener( new ChangeListener(){
            public void stateChanged(ChangeEvent changeEvent){
                y2 = ((Integer)y2spin.getValue()).intValue();
                interact( animation );
                animation.forceRedraw();
            }
        });
        final JSpinner forcespin = (JSpinner) engine.find( "force" );
        forcespin.setValue(new Integer(force));
        forcespin.addChangeListener( new ChangeListener(){
            public void stateChanged(ChangeEvent changeEvent){
                force = ((Integer)forcespin.getValue()).intValue();
            }
        });
        final JSpinner damagespin = (JSpinner) engine.find( "damage" );
        damagespin.setValue(new Integer(damage));
        damagespin.addChangeListener( new ChangeListener(){
            public void stateChanged(ChangeEvent changeEvent){
                damage = ((Integer)damagespin.getValue()).intValue();
            }
        });

        final JButton toggle = (JButton) engine.find("toggle");
        toggle.addActionListener(new AbstractAction(){
            boolean toggled = false;
            MouseInputAdapter listener = new MouseInputAdapter(){
                public void mousePressed(MouseEvent e){
                    x1 = (int)(e.getX() / area.getScale() - area.getCenterX() + animation.getWidth() / 2 - animation.getOffsetX());
                    y1 = (int)(e.getY() / area.getScale() - area.getCenterY() + animation.getHeight() - animation.getOffsetY());
                    x1spin.setValue(new Integer(x1));
                    y1spin.setValue(new Integer(y1));
                    interact(animation);
                    animation.forceRedraw();
                }

                public void mouseDragged(MouseEvent e){
                    x2 = (int)(e.getX() / area.getScale() - area.getCenterX() + animation.getWidth() / 2 - animation.getOffsetX());
                    y2 = (int)(e.getY() / area.getScale() - area.getCenterY() + animation.getHeight() - animation.getOffsetY());
                    x2spin.setValue(new Integer(x2));
                    y2spin.setValue(new Integer(y2));
                    interact(animation);
                    animation.forceRedraw();
                }
            };

            public void actionPerformed(ActionEvent event){
                if (toggled){
                    toggle.setText("Draw attack box");
                    area.enableMovement();
                    area.removeMouseListener(listener);
                    area.removeMouseMotionListener(listener);
                } else {
                    toggle.setText("Stop drawing");
                    area.disableMovement();
                    area.addMouseListener(listener);
                    area.addMouseMotionListener(listener);
                }

                toggled = ! toggled;
            }
        });

        return (JPanel)engine.getRootComponent();
    }

    public Token getToken(){
        Token temp = new Token("attack");
        if (x1 == 0 && y1 == 0 && x2 == 0 && y2 == 0 && force == 0 && damage == 0) {
            temp.addToken(new Token("attack"));
        } else {
            temp.addToken(new Token("attack"));
            temp.addToken(new String[]{"x1",Integer.toString(x1)});
            temp.addToken(new String[]{"y1",Integer.toString(y1)});
            temp.addToken(new String[]{"x2",Integer.toString(x2)});
            temp.addToken(new String[]{"y2",Integer.toString(y2)});
            temp.addToken(new String[]{"force",Integer.toString(force)});
            temp.addToken(new String[]{"damage",Integer.toString(damage)});
        }
        return temp;
    }
}
