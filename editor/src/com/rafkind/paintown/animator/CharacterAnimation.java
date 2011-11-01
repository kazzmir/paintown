package com.rafkind.paintown.animator;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.event.*;
import javax.swing.Timer;
import java.io.*;

import org.swixml.SwingEngine;

import com.rafkind.paintown.Lambda0;
import com.rafkind.paintown.Lambda1;
import com.rafkind.paintown.Lambda2;
import com.rafkind.paintown.Token;
import com.rafkind.paintown.exception.*;
import com.rafkind.paintown.RelativeFileChooser;
import com.rafkind.paintown.animator.DrawArea;
import com.rafkind.paintown.animator.DrawState;
import com.rafkind.paintown.animator.SpecialPanel;
import com.rafkind.paintown.animator.NewAnimator;
import com.rafkind.paintown.animator.events.AnimationEvent;
import com.rafkind.paintown.animator.events.EventFactory;
import com.rafkind.paintown.animator.events.OffsetEvent;

import java.awt.geom.AffineTransform;

import java.util.List;
import java.util.Vector;
import java.util.Map;
import java.util.HashMap;
import java.util.Iterator;
import java.util.ArrayList;

public class CharacterAnimation extends JPanel {

    /*
    private SwingEngine animEditor;
    private JTextField nameField;
    private BasicObject save;
    */

    /*
    public SpecialPanel getEditor(){	
        return new SpecialPanel((JPanel)animEditor.getRootComponent(),nameField, save );
    }
    */


    private boolean loaded = false;
    private Lambda0 loader;
    private final Animation animation;

    private synchronized boolean isLoaded(){
        return loaded;
    }

    private synchronized void doneLoading(){
        loaded = true;
    }

    public void paint(Graphics g){
        if (!isLoaded()){
            doneLoading();
            SwingUtilities.invokeLater(
                new Runnable(){
                    public void run(){
                        CharacterAnimation.this.loader.invoke_();
                        CharacterAnimation.this.revalidate();
                    }
                });
        }
        super.paint(g);
    }

    public CharacterAnimation(final AnimatedObject object, final Animation animation, final Lambda2 changeName){
        this.animation = animation;
        this.loader = new Lambda0(){
            public Object invoke() throws Exception {
                CharacterAnimation.this.setLayout(new GridBagLayout());

                JPanel area = new CharacterCanvas(object, animation, changeName);
                
                GridBagConstraints animConstraints = new GridBagConstraints();

                animConstraints.gridx = 0;
                animConstraints.gridy = 0;
                animConstraints.weightx = 1;
                animConstraints.weighty = 1;
                animConstraints.fill = GridBagConstraints.BOTH;
                animConstraints.anchor = GridBagConstraints.NORTHWEST;

                CharacterAnimation.this.add(area, animConstraints);
                return null;
            }
        };
    }

    public Animation getAnimation(){
        return animation;
    }

    private class CharacterCanvas extends AnimationCanvas {
        public CharacterCanvas(AnimatedObject object, Animation animation, Lambda2 changeName){
            super(object, animation, changeName);
        }

        protected JComponent makeProperties(final AnimatedObject object, final Animation animation, final Lambda2 changeName){
            final SwingEngine animEditor = new SwingEngine("animator/animation-properties.xml");

            final SwingEngine contextEditor = animEditor;
            final JTextField nameField = (JTextField) contextEditor.find("name");

            nameField.setText( animation.getName() );

            nameField.getDocument().addDocumentListener(new DocumentListener(){
                final CharacterAnimation self = CharacterAnimation.this;
                public void changedUpdate(DocumentEvent e){
                    animation.setName(nameField.getText());
                    changeName.invoke_(self, nameField.getText());
                }

                public void insertUpdate(DocumentEvent e){
                    animation.setName(nameField.getText());
                    changeName.invoke_(self, nameField.getText());
                }

                public void removeUpdate(DocumentEvent e){
                    animation.setName(nameField.getText());
                    changeName.invoke_(self, nameField.getText());
                }
            });

            final JComboBox typeCombo = (JComboBox) contextEditor.find("type");
            typeCombo.addItem("none");
            typeCombo.addItem("attack");
            typeCombo.addActionListener( new AbstractAction(){
                public void actionPerformed( ActionEvent event ){
                    animation.setType((String) typeCombo.getSelectedItem());
                }
            });
            typeCombo.setSelectedItem(animation.getType());

            final JList keyList = (JList) contextEditor.find("keys");
            final JComboBox keySelect = (JComboBox) contextEditor.find("key-select");

            keySelect.addItem("key_idle");
            keySelect.addItem("key_up");
            keySelect.addItem("key_down");
            keySelect.addItem("key_back");
            keySelect.addItem("key_forward");
            keySelect.addItem("key_upperback");
            keySelect.addItem("key_upperforward");
            keySelect.addItem("key_downback");
            keySelect.addItem("key_downforward");
            keySelect.addItem("key_jump");
            // keySelect.addItem("key_block");
            keySelect.addItem("key_attack1");
            keySelect.addItem("key_attack2");
            keySelect.addItem("key_attack3");
            keySelect.addItem("key_attack4");
            keySelect.addItem("key_attack5");
            keySelect.addItem("key_attack6");

            keyList.setListData(animation.getKeys());

            JButton keyAdd = (JButton) contextEditor.find("add-key");
            keyAdd.addActionListener(new AbstractAction(){
                public void actionPerformed( ActionEvent event ){
                    animation.addKey((String) keySelect.getSelectedItem());
                    keyList.setListData(animation.getKeys());
                }
            });
            JButton keyRemove = (JButton) contextEditor.find("remove-key");
            keyRemove.addActionListener(new AbstractAction(){
                public void actionPerformed(ActionEvent event){
                    if (! animation.getKeys().isEmpty()){
                        animation.removeKey(keyList.getSelectedIndex());
                        keyList.setListData(animation.getKeys());
                    }
                }
            });
            JButton keyUp = (JButton) contextEditor.find("up-key");
            keyUp.addActionListener( new AbstractAction(){
                public void actionPerformed(ActionEvent event){
                    if (! animation.getKeys().isEmpty()){
                        int index1 = keyList.getSelectedIndex()-1 < 0 ? 0 : keyList.getSelectedIndex() - 1;
                        int index2 = keyList.getSelectedIndex();
                        String temp1 = (String) animation.getKeys().elementAt( index1 );
                        String temp2 = (String) animation.getKeys().elementAt( index2 );

                        animation.getKeys().setElementAt(temp1,index2);
                        animation.getKeys().setElementAt(temp2,index1);
                        keyList.setListData(animation.getKeys());
                        keyList.setSelectedIndex(index1);
                    }
                }
            });

            JButton keyDown = (JButton) contextEditor.find("down-key");
            keyDown.addActionListener(new AbstractAction(){
                public void actionPerformed(ActionEvent event ){
                    if (! animation.getKeys().isEmpty()){
                        int index1 = keyList.getSelectedIndex() + 1 > animation.getKeys().size() ? animation.getKeys().size() : keyList.getSelectedIndex() + 1;
                        int index2 = keyList.getSelectedIndex();
                        String temp1 = (String) animation.getKeys().elementAt(index1);
                        String temp2 = (String) animation.getKeys().elementAt(index2);

                        animation.getKeys().setElementAt(temp1,index2);
                        animation.getKeys().setElementAt(temp2,index1);
                        keyList.setListData(animation.getKeys());
                        keyList.setSelectedIndex(index1);
                    }
                }
            });

            final JSpinner rangeSpinner = (JSpinner) contextEditor.find("range");
            rangeSpinner.setValue(new Integer(animation.getRange()));
            rangeSpinner.addChangeListener(new ChangeListener(){
                public void stateChanged(ChangeEvent changeEvent){
                    animation.setRange(((Integer)rangeSpinner.getValue()).intValue());
                    animation.forceRedraw();
                }
            });

            class SequenceModel implements ComboBoxModel {
                private List updates;
                private List animations;
                private Object selected;

                public SequenceModel(){
                    updates = new ArrayList();
                    animations = getAnimations( object );
                    selected = null;

                    object.addAnimationUpdate( new Lambda1(){
                        public Object invoke(Object o){
                            /* FIXME: is this an animated object? */
                            if (o instanceof CharacterStats){
                                CharacterStats who = (CharacterStats) o;
                                animations = new ArrayList();
                                animations = getAnimations(who);

                                updateAll();
                            }
                            return null;
                        }
                    });
                }

                private List getAnimations( AnimatedObject who ){
                    List all = new ArrayList();
                    Animation none = new Animation();
                    none.setName( "none" );
                    all.add( none );
                    all.addAll( who.getAnimations() );
                    for ( Iterator it = all.iterator(); it.hasNext(); ){
                        Animation updateAnimation = (Animation) it.next();
                        updateAnimation.addChangeUpdate( new Lambda1(){
                            public Object invoke( Object a ){
                                Animation ani = (Animation) a;
                                int index = animations.indexOf( ani );
                                if ( index != -1 ){
                                    ListDataEvent event = new ListDataEvent( this, ListDataEvent.CONTENTS_CHANGED, index, index );
                                    for ( Iterator it = updates.iterator(); it.hasNext(); ){
                                        ListDataListener l = (ListDataListener) it.next();
                                        l.contentsChanged( event );
                                    }
                                }
                                return null;
                            }
                        });
                    }
                    return all;
                }

                public void setSelectedItem( Object item ){
                    selected = item;
                    updateAll();
                }

                public Object getSelectedItem(){
                    return selected;
                }

                /* something changed.. notify listeners */
                private void updateAll(){
                    ListDataEvent event = new ListDataEvent( this, ListDataEvent.CONTENTS_CHANGED, 0, 999999 );
                    for ( Iterator it = updates.iterator(); it.hasNext(); ){
                        ListDataListener l = (ListDataListener) it.next();
                        l.contentsChanged( event );
                    }
                }

                public void addListDataListener( ListDataListener l ){
                    updates.add( l );
                }

                public void removeListDataListener( ListDataListener l ){
                    updates.remove( l );
                }

                public Object getElementAt( int index ){
                    return ((Animation) animations.get( index )).getName();
                }

                public int getSize(){
                    return animations.size();
                }
            }
            final JComboBox sequence = (JComboBox) contextEditor.find("sequence");
            sequence.setModel( new SequenceModel() );
            /*
               sequence.getModel().addListDataListener( new ListDataListener(){
               public void contentsChanged( ListDataEvent e ){
               int i = sequence.getSelectedIndex();

               if ( i > sequence.getModel().getSize() ){
               i = sequence.getModel().getSize() - 1;
               }

               if ( i <= 0 ){
               i = 0;
               }

               System.out.println( "Check " + i + " " + sequence.getItemAt( i ) + " vs " + animation.getSequence() );
               if ( ! sequence.getItemAt( i ).equals( animation.getSequence() ) ){
               animation.setSequence( (String) sequence.getItemAt( i ) );
               }

               sequence.setSelectedItem( animation.getSequence() );
               }

               public void intervalAdded( ListDataEvent e ){
               }

               public void intervalRemoved( ListDataEvent e ){
               }
               });
               */
            /*
               sequence.addItem( "none" );
               for ( Iterator it = character.getAnimations().iterator(); it.hasNext(); ){
               Animation ani = (Animation) it.next();
               if ( ! ani.getName().equals( animation.getName() ) ){
               sequence.addItem( ani.getName() );
               }
               }
               */
            sequence.addActionListener( new AbstractAction(){
                public void actionPerformed( ActionEvent event ){
                    animation.setSequence( (String) sequence.getSelectedItem() );
                }
            });
            sequence.setSelectedItem( animation.getSequence() );

            final JTextField basedirField = (JTextField) contextEditor.find("basedir");
            {
                Dimension size = basedirField.getMinimumSize();
                size.setSize(9999999, size.getHeight());
                basedirField.setMaximumSize(size);
            }
            basedirField.setText(animation.getBaseDirectory());
            JButton basedirButton = (JButton) contextEditor.find("change-basedir");
            basedirButton.addActionListener(new AbstractAction(){
                public void actionPerformed(ActionEvent event){
                    RelativeFileChooser chooser = NewAnimator.getNewFileChooser(object.getPath());
                    int ret = chooser.open();
                    if (ret == RelativeFileChooser.OK){
                        final String path = chooser.getPath();
                        basedirField.setText(path);
                        animation.setBaseDirectory(path);
                    }
                }
            });

            return (JComponent) animEditor.getRootComponent();
        }
    }

    private void debugSwixml( SwingEngine engine ){
        Map all = engine.getIdMap();
        System.out.println( "Debugging swixml" );
        for ( Iterator it = all.entrySet().iterator(); it.hasNext(); ){
            Map.Entry entry = (Map.Entry) it.next();
            System.out.println( "Id: " + entry.getKey() + " = " + entry.getValue() );
        }
    }
}
