package com.rafkind.paintown.animator;

import java.util.*;
import javax.swing.*;
import java.awt.*;
import javax.swing.*;
import java.awt.image.*;
import java.awt.geom.*;
import java.awt.event.*;
import javax.swing.Timer;
import javax.swing.event.*;

import org.swixml.SwingEngine;

import com.rafkind.paintown.animator.*;
import com.rafkind.paintown.animator.events.AnimationEvent;
import com.rafkind.paintown.*;

public final class DrawArea extends JComponent {
	private int x = 50;
	private int y = 50;
	private double scale;
	
	private AnimationEvent currentEvent;
	private Animation currentAnimation;

	public DrawArea(final Lambda0 loader){
            setFocusable(true);
		currentAnimation = null;

		scale = 1.0;

		this.addMouseMotionListener( new MouseMotionAdapter(){
                    public void mouseClicked(MouseEvent e){
                        requestFocusInWindow();
                    }

                    public void mouseDragged( MouseEvent event ){
                        x = (int)(event.getX() / getScale());
                        y = (int)(event.getY() / getScale());
                        DrawArea.this.repaint();
                    }
		});

                class PopupHack{
                    private Popup box;
                    private int counter;
                    public PopupHack(JComponent insides){
                        Point here = DrawArea.this.getLocation();
                        SwingUtilities.convertPointToScreen(here, DrawArea.this);
                        box = PopupFactory.getSharedInstance().getPopup(DrawArea.this, insides, (int) here.getX(), (int) here.getY());
                        box.show();
                        counter = 10;
                    }

                    public void hide(){
                        box.hide();
                    }

                    public boolean decrement(){
                        counter -= 1;
                        return counter <= 0;
                    }

                    public void pressed(){
                        counter = 10;
                    }
                }

                final PopupHack[] box = new PopupHack[1];

                this.getInputMap().put( KeyStroke.getKeyStroke( KeyEvent.VK_Q, 0, false ), "press" );
		this.getActionMap().put( "press", new AbstractAction(){
                    public void actionPerformed( ActionEvent event ){
                        boolean ok = false;
                        synchronized(box){
                            ok = box[0] == null;
                        }
                        if ( ok ){
                            /*
                            System.out.println("Pressed a key");
                            JButton button = new JButton("hello!");
                            button.setSize(100,100);
                            */
                            JComponent input = (JComponent) loader.invoke_();
                            if ( input == null ){
                                return;
                            }
                            synchronized(box){
                                box[0] = new PopupHack(input);
                            }
                            final Timer t = new Timer(500, new ActionListener(){
                                public void actionPerformed( ActionEvent event ){
                                }
                            });
                            t.addActionListener(new ActionListener(){
                                public void actionPerformed(ActionEvent event){
                                    synchronized(box){
                                        if ( box[0] != null && box[0].decrement() ){
                                            box[0].hide();
                                            box[0] = null;
                                            t.stop();
                                        }
                                    }
                                }
                            });
                            t.start();
                        } else {
                            synchronized(box){
                                box[0].pressed();
                            }
                        }
                    }
		});

                /*
                this.addKeyListener(new KeyListener(){
                    private Popup box;
                    public void keyPressed(KeyEvent e){
                        if ( box == null ){
                            System.out.println("Pressed a key");
                            jbutton button = new jbutton("hello!");
                            button.setsize(100,100);
                            box = popupfactory.getsharedinstance().getpopup(drawarea.this, button, getx(), gety());
                            box.show();
                        }
                    }

                    public void keyReleased(KeyEvent e){
                        if ( box != null ){
                            System.out.println("Key released");
                            box.hide();
                            box = null;
                        }
                    }

                    public void keyTyped(KeyEvent e){
                    }
                });
                */
	}

	public double getScale(){
		return scale;
	}

	public void setScale( double x ){
		scale = x;
		repaint();
	}

        /*
	public Dimension getPreferredSize(){
		return new Dimension(800,600);
	}
        */

	public void paintComponent( Graphics g ){

		Graphics2D g2d = (Graphics2D) g;
		g2d.scale( getScale(), getScale() );

		g.setColor( new Color( 0, 0, 0 ) );
		g.fillRect( 0, 0, getWidth(), getHeight() );
		g.setColor( new Color( 255, 255, 0 ) );
		g.drawLine( 0, y, getWidth(), y );
		g.drawLine( x, 0, x, getHeight() );

		if ( currentAnimation != null ){
			currentAnimation.draw( g, x, y - currentAnimation.getHeight() );
		}
	}

	public void animate( Animation animation ){
		unanimate();
		currentAnimation = animation;
		currentAnimation.addDrawable( this );
	}

	public void unanimate(){
		if ( currentAnimation != null ){
			currentAnimation.removeDrawable( this );
		}
	}
	
	public AnimationEvent getCurrentEvent(){
		return currentEvent;
	}
}
