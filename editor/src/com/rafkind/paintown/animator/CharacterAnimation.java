package com.rafkind.paintown.animator;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.event.*;
import javax.swing.Timer;
import java.io.*;

import scala.collection.JavaConversions;

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
import com.rafkind.paintown.animator.Animator;
import com.rafkind.paintown.animator.events.AnimationEvent;
import com.rafkind.paintown.animator.events.EventFactory;
import com.rafkind.paintown.animator.events.FrameEvent;
import com.rafkind.paintown.animator.events.OffsetEvent;

import java.awt.geom.AffineTransform;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.DataFlavor;
import javax.activation.DataHandler;

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

    private class ObjectBox{
        public ObjectBox(){}
        public synchronized void set( Object x ){ obj = x; }
        public synchronized Object get(){ return obj; }
        private Object obj;
    }

    private boolean rightClick( MouseEvent event ){
        return event.getButton() == MouseEvent.BUTTON3;
    }

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

                GridBagConstraints animConstraints = new GridBagConstraints();

                animConstraints.gridx = 0;
                animConstraints.gridy = 0;
                animConstraints.weightx = 1;
                animConstraints.weighty = 1;
                animConstraints.fill = GridBagConstraints.BOTH;
                animConstraints.anchor = GridBagConstraints.NORTHWEST;

                final SwingEngine animEditor = new SwingEngine("animator/animation.xml");
                CharacterAnimation.this.add((JPanel) animEditor.getRootComponent(), animConstraints);

                final JTabbedPane tabs = (JTabbedPane) animEditor.find("tabs");

                final DrawArea area = new DrawArea(new Lambda0(){
                    public Object invoke(){
                        /*
                           if ( eventList.getSelectedIndex() != -1 ){
                           AnimationEvent event = (AnimationEvent) eventList.getSelectedValue();
                           return event.getEditor(animation, );
                           } else {
                           return null;
                           }
                           */

                        return null;
                    }
                });

                tabs.addTab("Properties", makeProperties(object, animation, changeName));
                tabs.addTab("Events", makeEvents(animation, area));
                // this.save = object;
                
                JPanel canvas = (JPanel) animEditor.find("canvas");
                GridBagConstraints constraints = new GridBagConstraints();
                constraints.gridx = 0;
                constraints.gridy = 0;
                constraints.weightx = 1;
                constraints.weighty = 1;
                constraints.fill = GridBagConstraints.BOTH;
                constraints.anchor = GridBagConstraints.NORTHWEST;

                canvas.add(area, constraints);

                final JSplitPane split = (JSplitPane) animEditor.find("split");
                SwingUtilities.invokeLater(new Runnable(){
                    public void run(){
                        /* hack to set the divider location */
                        if (split.getWidth() != 0){
                            split.setDividerLocation(0.6);
                        } else {
                            SwingUtilities.invokeLater(this);
                        }
                    }
                });
                // split.setDividerLocation(0.6);

                // SwingEngine contextEditor = new SwingEngine ( "animator/animation.xml");
                final SwingEngine contextEditor = animEditor;

                SwingEngine controlEditor = new SwingEngine("animator/controls.xml");

                JPanel controls = (JPanel) animEditor.find( "controls" );

                JButton displayToken = (JButton) controlEditor.find( "token" );

                displayToken.addActionListener( new AbstractAction(){
                    public void actionPerformed( ActionEvent event ){
                        final JDialog tempDiag = new JDialog();
                        tempDiag.setSize(400,400);
                        final JTextArea tempText = new JTextArea();
                        final JScrollPane tempPane = new JScrollPane(tempText);
                        tempDiag.add(tempPane);
                        tempText.setText( animation.getToken().toString());
                        tempDiag.show();
                    }
                });

                JButton stopAnim = (JButton) animEditor.find( "stop" );
                stopAnim.addActionListener( new AbstractAction(){
                    public void actionPerformed( ActionEvent event ){
                        animation.stopRunning();
                    }
                });

                JButton playAnim = (JButton) animEditor.find( "play" );
                playAnim.addActionListener( new AbstractAction(){
                    public void actionPerformed( ActionEvent event ){
                        animation.startRunning();
                    }
                });

                JButton previousFrame = (JButton) animEditor.find("previous-frame");
                JButton nextFrame = (JButton) animEditor.find("next-frame");

                previousFrame.addActionListener(new AbstractAction(){
                    public void actionPerformed(ActionEvent event){
                        animation.previousFrame();
                        animation.forceRedraw();
                    }
                });

                nextFrame.addActionListener(new AbstractAction(){
                    public void actionPerformed(ActionEvent event){
                        animation.nextFrame();
                        animation.forceRedraw();
                    }
                });

                final JComboBox tools = (JComboBox) contextEditor.find("tools");
                final JPanel toolPane = (JPanel) contextEditor.find("tool-area");
                final String chooseNone = "None";
                final String chooseBackground = "Background Color";
                final String chooseGrid = "Grid";
                tools.addItem(chooseNone);
                tools.addItem(chooseBackground);
                tools.addItem(chooseGrid);

                tools.addActionListener(new AbstractAction(){
                    final JPanel toolNone = new JPanel();
                    final JPanel toolBackground = makeBackgroundTool(area, animation);
                    final JPanel toolGrid = makeGridTool(area, animation);

                    private JPanel getTool(String name){
                        if (name.equals(chooseNone)){
                            return toolNone;
                        }
                        if (name.equals(chooseBackground)){
                            return toolBackground;
                        }
                        if (name.equals(chooseGrid)){
                            return toolGrid;
                        }

                        throw new RuntimeException("No such tool with name '" + name + "'");
                    }

                    public void actionPerformed(ActionEvent event){
                        toolPane.removeAll();
                        GridBagConstraints constraints = new GridBagConstraints();
                        constraints.gridx = 0;
                        constraints.gridy = 0;
                        constraints.weightx = 1;
                        constraints.weighty = 1;
                        constraints.fill = GridBagConstraints.NONE;
                        constraints.anchor = GridBagConstraints.NORTHWEST;
                        toolPane.add(getTool((String) tools.getSelectedItem()), constraints);
                        toolPane.revalidate();
                    }
                });

                final JLabel animationSpeed = (JLabel) contextEditor.find("speed-num");
                animationSpeed.setText( "Animation speed: " + animation.getAnimationSpeed() );
                final JSlider speed = (JSlider) contextEditor.find("speed");
                final double speedNumerator = 20.0;
                speed.setValue( (int) (speedNumerator / animation.getAnimationSpeed()) );
                speed.addChangeListener( new ChangeListener(){
                    public void stateChanged( ChangeEvent e ){
                        animation.setAnimationSpeed(speedNumerator / speed.getValue());
                        animationSpeed.setText("Animation speed: " + speed.getValue() / speedNumerator);
                    }
                });

                final JButton speedIncrease = (JButton) contextEditor.find("speed:increase");
                final JButton speedDecrease = (JButton) contextEditor.find("speed:decrease");

                speedIncrease.addActionListener(new AbstractAction(){
                    public void actionPerformed(ActionEvent event){
                        adjustSlider(speed, +1);
                    }
                });

                speedDecrease.addActionListener(new AbstractAction(){
                    public void actionPerformed(ActionEvent event){
                        adjustSlider(speed, -1);
                    }
                });

                // controls.add((JComponent)controlEditor.getRootComponent());

                
                final JLabel scaleNum = (JLabel) animEditor.find( "scale-num" );
                scaleNum.setText( "Scale: " + area.getScale() );
                final JSlider scale = (JSlider) animEditor.find( "scale" );
                scale.setValue( (int)(area.getScale() * 5.0) );
                scale.addChangeListener( new ChangeListener(){
                    public void stateChanged( ChangeEvent e ){
                        area.setScale( scale.getValue() / 5.0 );
                        scaleNum.setText( "Scale: " + area.getScale() );
                    }
                });

                final JButton scaleIncrease = (JButton) animEditor.find("scale:increase");
                final JButton scaleDecrease = (JButton) animEditor.find("scale:decrease");

                scaleIncrease.addActionListener(new AbstractAction(){
                    public void actionPerformed(ActionEvent event){
                        adjustSlider(scale, +1);
                    }
                });

                scaleDecrease.addActionListener(new AbstractAction(){
                    public void actionPerformed(ActionEvent event){
                        adjustSlider(scale, -1);
                    }
                });

                area.animate(animation);

                // context.add((JComponent)contextEditor.getRootComponent());
                return null;
            }
        };
    }

    public Animation getAnimation(){
        return animation;
    }

    private JPanel makeGridTool(final DrawArea area, final Animation animation){
        final SwingEngine context = new SwingEngine("animator/animation-tools.xml");
        final JSlider guide = (JSlider) context.find("guide");
        guide.setValue(area.getGuideSize());
        guide.addChangeListener(new ChangeListener(){
            public void stateChanged(ChangeEvent change){
                area.setGuideSize(guide.getValue());
                animation.forceRedraw();
            }
        });
        return (JPanel) context.find("grid");
    }

    private JPanel makeBackgroundTool(final DrawArea area, final Animation animation){
        final SwingEngine context = new SwingEngine("animator/animation-tools.xml");
        final JButton blackBackground = (JButton) context.find("black-background");
        final JButton whiteBackground = (JButton) context.find("white-background");
        blackBackground.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                area.setBackgroundColor(new Color(0, 0, 0));
                animation.forceRedraw();
            }
        });

        whiteBackground.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                area.setBackgroundColor(new Color(255, 255, 255));
                animation.forceRedraw();
            }
        });
        
        return (JPanel) context.find("background");
    }

    private JComponent makeProperties(final AnimatedObject object, final Animation animation, final Lambda2 changeName){
        final SwingEngine animEditor = new SwingEngine("animator/animation-properties.xml");

        // final JButton hide = (JButton) animEditor.find("hide");
        final Box upper = (Box) animEditor.find("upper");
        /*
        hide.addActionListener(new AbstractAction(){
            boolean show = true;
            final Icon original = hide.getIcon();
            private Icon make(final ObjectBox rotate){
                return new Icon(){
                    public int getIconHeight(){
                        return original.getIconHeight();
                    }

                    public int getIconWidth(){
                        return original.getIconWidth();
                    }

                    public void paintIcon(Component c, Graphics g, int x, int y){
                        Double d = (Double) rotate.get();
                        Graphics2D g2 = (Graphics2D) g;
                        / * translate back * /
                        g2.translate(x + -Math.cos(d.doubleValue()) * getIconWidth() + getIconWidth() / 2, y);
                        / * rotate * /
                        g2.rotate(d.doubleValue());
                        / * translate to origin * /
                        g2.translate(-x, -y);
                        original.paintIcon(c, g, x, y);
                    }
                };
            }

            public void actionPerformed(ActionEvent event){
                show = ! show;
                upper.setVisible(show);
                double start = 0;
                double end = Math.PI / 2;
                int direction = 1;
                if (show){
                    start = Math.PI / 2;
                    end = 0;
                    direction = -direction;
                }

                final ObjectBox value = new ObjectBox();
                value.set(new Double(start));

                hide.setIcon(make(value));

                final double x_start = start;
                final double x_end = end;
                final int x_direction = direction;
                final int speed = 20;
                new Thread(){
                    private void rest( int m ){
                        try{
                            Thread.sleep( m );
                        } catch ( Exception e ){
                        }
                    }

                    public void run(){
                        boolean done = false;
                        while (! done){
                            double rotate = ((Double) value.get()).doubleValue();
                            rotate += 0.1 * x_direction;
                            if (x_direction > 0 && rotate >= x_end){
                                done = true;
                                rotate = x_end;
                            } else if (x_direction < 0 && rotate <= x_end){
                                done = true;
                                rotate = x_end;
                            }
                            value.set(new Double(rotate));
                            hide.repaint();
                            rest(speed);
                        }
                    }
                }.start();
            }
        });
        */

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

        final JComboBox typeCombo = (JComboBox) contextEditor.find( "type" );
        typeCombo.addItem("none");
        typeCombo.addItem("attack");
        typeCombo.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                animation.setType((String) typeCombo.getSelectedItem());
            }
        });
        typeCombo.setSelectedItem( animation.getType() );

        final JList keyList = (JList) contextEditor.find( "keys");
        final JComboBox keySelect = (JComboBox) contextEditor.find( "key-select" );

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

        keyList.setListData( animation.getKeys() );

        JButton keyAdd = (JButton) contextEditor.find( "add-key" );
        keyAdd.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                animation.addKey( (String) keySelect.getSelectedItem() );
                keyList.setListData( animation.getKeys() );
            }
        });
        JButton keyRemove = (JButton) contextEditor.find( "remove-key" );
        keyRemove.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                if ( ! animation.getKeys().isEmpty() ){
                    animation.removeKey( keyList.getSelectedIndex() );
                    keyList.setListData( animation.getKeys() );
                }
            }
        });
        JButton keyUp = (JButton) contextEditor.find( "up-key" );
        keyUp.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                if ( ! animation.getKeys().isEmpty() ){
                    int index1 = keyList.getSelectedIndex()-1 < 0 ? 0 : keyList.getSelectedIndex() - 1;
                    int index2 = keyList.getSelectedIndex();
                    String temp1 = (String) animation.getKeys().elementAt( index1 );
                    String temp2 = (String) animation.getKeys().elementAt( index2 );

                    animation.getKeys().setElementAt(temp1,index2);
                    animation.getKeys().setElementAt(temp2,index1);
                    keyList.setListData( animation.getKeys() );
                    keyList.setSelectedIndex( index1 );
                }
            }
        });

        JButton keyDown = (JButton) contextEditor.find( "down-key" );
        keyDown.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                if ( ! animation.getKeys().isEmpty() ){
                    int index1 = keyList.getSelectedIndex()+1 > animation.getKeys().size() ? animation.getKeys().size() : keyList.getSelectedIndex() + 1;
                    int index2 = keyList.getSelectedIndex();
                    String temp1 = (String) animation.getKeys().elementAt( index1 );
                    String temp2 = (String) animation.getKeys().elementAt( index2 );

                    animation.getKeys().setElementAt(temp1,index2);
                    animation.getKeys().setElementAt(temp2,index1);
                    keyList.setListData( animation.getKeys() );
                    keyList.setSelectedIndex( index1 );
                }
            }
        });

        final JSpinner rangeSpinner = (JSpinner) contextEditor.find( "range" );
        rangeSpinner.setValue( new Integer( animation.getRange() ) );
        rangeSpinner.addChangeListener( new ChangeListener(){
            public void stateChanged(ChangeEvent changeEvent){
                animation.setRange( ((Integer)rangeSpinner.getValue()).intValue() );
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
                    public Object invoke( Object o ){
                        CharacterStats who = (CharacterStats) o;
                        animations = new ArrayList();
                        animations = getAnimations( who );

                        updateAll();
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
        final JComboBox sequence = (JComboBox) contextEditor.find( "sequence" );
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

        final JTextField basedirField = (JTextField) contextEditor.find( "basedir" );
        {
            Dimension size = basedirField.getMinimumSize();
            size.setSize(9999999, size.getHeight());
            basedirField.setMaximumSize(size);
        }
        basedirField.setText( animation.getBaseDirectory() );
        JButton basedirButton = (JButton) contextEditor.find( "change-basedir" );
        basedirButton.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                RelativeFileChooser chooser = Animator.getNewFileChooser();
                int ret = chooser.open();
                if ( ret == RelativeFileChooser.OK ){
                    final String path = chooser.getPath();
                    basedirField.setText( path );
                    animation.setBaseDirectory( path );
                }
            }
        });

        return (JComponent) animEditor.getRootComponent();
    }

    private JComponent makeEvents(final Animation animation, final DrawArea area){
        final SwingEngine contextEditor = new SwingEngine("animator/animation-events.xml");
        final JScrollPane eventScroll = (JScrollPane) contextEditor.find("event-scroll");
        // eventScroll.setComponentOrientation(ComponentOrientation.RIGHT_TO_LEFT);

        final JList eventList = (JList) contextEditor.find("events");
        eventList.setDragEnabled(true);
        eventList.setListData(animation.getEvents());
        eventList.setDropMode(DropMode.ON);
        eventList.setTransferHandler(new TransferHandler(){
            private int toRemove = -1;
            /*
               public boolean canImport(JComponent comp, DataFlavor[] transferFlavors){
               System.out.println("Can I import " + transferFlavors);
               return true;
               }
               */

            public boolean canImport(TransferHandler.TransferSupport support){
                // System.out.println("Can I import " + support);
                /* bleh */
                return true;
            }

            /*
               public void exportAsDrag(JComponent component, InputEvent event, int action){
               System.out.println("export event " + event + " action " + action + " move is " + TransferHandler.MOVE);
               super.exportAsDrag(component, event, action);
               }
               */

            protected void exportDone(JComponent source, Transferable data, int action){
                // System.out.println("did export action was " + action);
                if (action == TransferHandler.MOVE){
                    /*
                       System.out.println("Transfered " + data);

                       int index = 0;
                       if (eventList.getSelectedIndex() != -1){
                       AnimationEvent event = 
                       animation.removeEvent(eventList.getSelectedIndex() );
                       index = animation.addEvent(temp, eventList.getSelectedIndex() + 1);
                       }
                       eventList.setListData( animation.getEvents() );
                       eventList.setSelectedIndex( index );
                       */
                    // System.out.println("Removing event " + toRemove);
                    animation.removeEvent(toRemove);
                    eventList.setListData(animation.getEvents());
                }
            }

            public int getSourceActions(JComponent component){
                // System.out.println("What are my actions? " + TransferHandler.MOVE);
                return TransferHandler.MOVE;
            }

            protected Transferable createTransferable(JComponent component){
                // System.out.println("make a transferable");
                return new Transferable(){
                    final Object index = eventList.getSelectedValue();
                    public Object getTransferData(DataFlavor flavor){
                        return index;
                    }

                    public DataFlavor[] getTransferDataFlavors(){
                        try{
                            DataFlavor[] flavors = new DataFlavor[1];
                            flavors[0] = new DataFlavor(DataFlavor.javaJVMLocalObjectMimeType);
                            return flavors;
                        } catch (ClassNotFoundException ex){
                            System.out.println(ex);
                            return null;
                        }
                    }

                    public boolean isDataFlavorSupported(DataFlavor flavor){
                        return true;
                    }
                };
                /*
                   return new DataHandler(new Integer(eventList.getSelectedIndex()), "event-list");
                   */
            }

            public boolean importData(TransferSupport supp) {
                // System.out.println("Try to import " + supp);
                if (!canImport(supp)) {
                    return false;
                }

                try{
                    JList.DropLocation location = (JList.DropLocation) supp.getDropLocation();
                    // System.out.println("Drop location to " + location);
                    toRemove = eventList.getSelectedIndex();
                    int index = location.getIndex();
                    if (index < toRemove){
                        toRemove += 1;
                    }
                    AnimationEvent event = (AnimationEvent) supp.getTransferable().getTransferData(new DataFlavor(DataFlavor.javaJVMLocalObjectMimeType));
                    animation.addEvent(event, index);
                    eventList.setListData(animation.getEvents());
                    eventList.setSelectedIndex(index);
                } catch (ClassNotFoundException exception){
                    System.err.println(exception);
                } catch (java.awt.datatransfer.UnsupportedFlavorException exception){
                    System.err.println(exception);
                } catch (IOException exception){
                    System.err.println(exception);
                }

                /*
                   int index = 0;
                   if (eventList.getSelectedIndex() != -1){
                   AnimationEvent event = animation.removeEvent(eventList.getSelectedIndex() );
                   index = animation.addEvent(temp, eventList.getSelectedIndex() + 1);
                   }
                   eventList.setListData( animation.getEvents() );
                   eventList.setSelectedIndex( index );
                   */

                /*
                // Fetch the Transferable and its data
                Transferable t = supp.getTransferable();
                String data = t.getTransferData(stringFlavor);

                // Fetch the drop location
                DropLocation loc = supp.getDropLocation();

                // Insert the data at this location
                insertAt(loc, data);
                */

                return true;
            }

        });

        final ObjectBox currentEvent = new ObjectBox();

        animation.addEventNotifier( new Lambda1(){
            public Object invoke( Object a ){
                currentEvent.set( a );
                eventList.repaint();
                return null;
            }
        });

        eventList.addListSelectionListener(new ListSelectionListener(){
            public void valueChanged(ListSelectionEvent e){
                AnimationEvent event = (AnimationEvent) eventList.getSelectedValue();
                animation.stopRunning();
                animation.nextEvent( event );
                currentEvent.set( event );
            }
        });

        final ObjectBox lastEvent = new ObjectBox();
        final Lambda1 doEvent = new Lambda1(){
            final JPanel work = (JPanel) contextEditor.find("event-work");
            public Object invoke(Object _event){
                if (lastEvent.get() != null){
                    AnimationEvent last = (AnimationEvent) lastEvent.get();
                    last.destroy();
                }
                AnimationEvent event = (AnimationEvent) _event;
                lastEvent.set(event);
                JPanel editor = event.getEditor(animation, area);
                work.removeAll();
                GridBagConstraints constraints = new GridBagConstraints();
                constraints.gridx = 0;
                constraints.gridy = 0;
                constraints.weightx = 1;
                constraints.weighty = 1;
                constraints.fill = GridBagConstraints.BOTH;
                constraints.anchor = GridBagConstraints.NORTHWEST;
                work.add(editor, constraints);
                work.revalidate();
                return null;
            }
        };

        eventList.addMouseListener( new MouseAdapter() {
            public void mouseClicked(MouseEvent e) {
                if ( rightClick( e ) || e.getClickCount() == 2 ){
                    int index = eventList.locationToIndex(e.getPoint());
                    AnimationEvent event = (AnimationEvent) animation.getEvents().elementAt(index);
                    doEvent.invoke_(event);
                    /*
                       JPanel editor = event.getEditor(animation);
                       JPanel work = (JPanel) contextEditor.find("event-work");
                       work.removeAll();
                       GridBagConstraints constraints = new GridBagConstraints();
                       constraints.gridx = 0;
                       constraints.gridy = 0;
                       constraints.weightx = 1;
                       constraints.weighty = 1;
                       constraints.fill = GridBagConstraints.BOTH;
                       constraints.anchor = GridBagConstraints.NORTHWEST;
                       work.add(editor, constraints);
                       work.revalidate();
                       */

                    /*
                       if ( editor != null ){
                       JDialog dialog = new JDialog();
                       dialog.setSize(editor.getSize());
                       Point here = animEditor.getRootComponent().getLocation();
                       SwingUtilities.convertPointToScreen(here, animEditor.getRootComponent());
                       here.setLocation(here.getX() + animEditor.getRootComponent().getWidth() / 2, here.getY() + animEditor.getRootComponent().getHeight() / 2);
                       dialog.setLocation(here);
                       dialog.getContentPane().add(editor);
                       dialog.addWindowStateListener(new WindowStateListener(){
                       public void windowStateChanged(WindowEvent e){
                       / * should use a list update event here * /
                       eventList.setListData( animation.getEvents() );
                       }
                       });
                       dialog.show();
                       }
                       */
                }
            }
        });

        eventList.setCellRenderer(new DefaultListCellRenderer() {
            public Component getListCellRendererComponent(
                JList list,
                Object value,
                int index,
                boolean isSelected,
                boolean cellHasFocus){

                setText(((AnimationEvent)value).getName());
                setBackground(isSelected ? Color.gray : Color.white);
                if ( currentEvent.get() == value ){
                    setForeground( Color.blue );
                } else {
                    setForeground(isSelected ? Color.white : Color.black);
                }
                return this;
                }
        });

        // Need to add events to this combobox from event factory
        // EventFactory.init();
        final JComboBox eventSelect = (JComboBox) contextEditor.find( "event-select" );
        for (Iterator it = EventFactory.getNames().iterator(); it.hasNext();){
            String event = (String) it.next();
            eventSelect.addItem(event);
        }

        final JComboBox tools = (JComboBox) contextEditor.find("tools");
        final JPanel toolPane = (JPanel) contextEditor.find("tool-area");
        final String chooseNone = "None";
        final String chooseOnionSkinning = "Onion Skinning";
        final String chooseAdjustOffsets = "AdjustOffsets";
        tools.addItem(chooseNone);
        tools.addItem(chooseOnionSkinning);
        tools.addItem(chooseAdjustOffsets);

        tools.addActionListener(new AbstractAction(){
            final JPanel toolNone = new JPanel();
            final JPanel toolOnionSkinning = makeOnionSkinningPanel(animation);
            final JPanel toolAdjustOffsets = makeAdjustOffsetsPanel(animation);

            private JPanel getTool(String name){
                if (name.equals(chooseNone)){
                    return toolNone;
                }
                if (name.equals(chooseOnionSkinning)){
                    return toolOnionSkinning;
                }
                if (name.equals(chooseAdjustOffsets)){
                    return toolAdjustOffsets;
                }

                throw new RuntimeException("No such tool with name '" + name + "'");
            }

            public void actionPerformed(ActionEvent event){
                toolPane.removeAll();
                GridBagConstraints constraints = new GridBagConstraints();
                constraints.gridx = 0;
                constraints.gridy = 0;
                constraints.weightx = 1;
                constraints.weighty = 1;
                constraints.fill = GridBagConstraints.NONE;
                constraints.anchor = GridBagConstraints.NORTHWEST;
                toolPane.add(getTool((String) tools.getSelectedItem()), constraints);
                toolPane.revalidate();

            }
        });

        /*
           JButton adjustOffsets = (JButton) contextEditor.find("adjust-offsets");
           adjustOffsets.addActionListener(new AbstractAction(){
           public void actionPerformed(ActionEvent event){
           showAdjustOffsetsDialog(animation, animation.getEvents());
           eventList.repaint();
           }
           });
           */

        JButton addAllFrames = (JButton) contextEditor.find("add-frames");
        addAllFrames.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                for (String path: JavaConversions.asJavaCollection(NewAnimator.getFiles(animation.getBaseDirectory()))){
                    com.rafkind.paintown.animator.events.scala.FrameEvent frame = new com.rafkind.paintown.animator.events.scala.FrameEvent();
                    frame.setFrame(path);
                    doEvent.invoke_(frame);
                    animation.addEvent(frame);
                }
                eventList.setListData(animation.getEvents());
            }
        });

        JButton eventView = (JButton) contextEditor.find("view-events");
        eventView.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                final SwingEngine engine = new SwingEngine("animator/view-events.xml");
                // final JDialog dialog = new JDialog("Event viewer");
                final JDialog dialog = new JDialog();

                final JButton close = (JButton) engine.find("close");
                close.addActionListener( new AbstractAction(){
                    public void actionPerformed(ActionEvent event ){
                        dialog.setVisible(false);
                    }
                });

                final JPanel work = (JPanel) engine.find("work");

                final Lambda1 update = new Lambda1(){
                    /* multiple requests should really just be folded
                     * into a single call to this function.
                     */
                    private synchronized void doit(){
                        int x = 0;
                        int y = 0;
                        work.removeAll();
                        double maxWidth = Math.sqrt(animation.getEvents().size());
                        for (AnimationEvent aevent : animation.getEvents()){
                            JPanel editor = aevent.getEditor(animation, area);
                            // editor.setSize(50, 50);
                            GridBagConstraints constraints = new GridBagConstraints();
                            constraints.gridx = x;
                            constraints.gridy = y;
                            constraints.weightx = 0.1;
                            constraints.weighty = 0.1;
                            constraints.fill = GridBagConstraints.BOTH;
                            // constraints.anchor = GridBagConstraints.NORTHWEST;
                            work.add(editor, constraints);
                            x += 1;
                            if (x > maxWidth){
                                y += 1;
                                x = 0;
                            }
                        }
                        work.revalidate();
                    }

                    public Object invoke(Object o){
                        SwingUtilities.invokeLater(new Runnable(){
                            public void run(){
                                doit();
                            }
                        });
                        return null;
                    }
                };

                update.invoke_(null);
                animation.addChangeUpdate(update);

                dialog.getContentPane().add((JPanel) engine.getRootComponent());
                dialog.setSize(300, 300);
                dialog.setVisible(true);
            }
        });

        JButton eventAdd = (JButton) contextEditor.find( "add-event" );
        eventAdd.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                AnimationEvent temp = EventFactory.getEvent((String)eventSelect.getSelectedItem());
                doEvent.invoke_(temp);
                int index = 0;
                if (eventList.getSelectedIndex() != -1){
                    index = animation.addEvent(temp, eventList.getSelectedIndex() + 1);
                } else {
                    index = animation.addEvent(temp);
                }
                eventList.setListData(animation.getEvents());
                eventList.setSelectedIndex(index);
            }
        });

        JButton eventEdit = (JButton) contextEditor.find( "edit-event" );
        eventEdit.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                if( ! animation.getEvents().isEmpty()){
                    AnimationEvent temp = (AnimationEvent) animation.getEvents().elementAt( eventList.getSelectedIndex() );
                    doEvent.invoke_(temp);
                }
            }
        });

        JButton eventRemove = (JButton) contextEditor.find( "remove-event" );
        eventRemove.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                if ( ! animation.getEvents().isEmpty() ){
                    animation.removeEvent( eventList.getSelectedIndex() );
                    eventList.setListData( animation.getEvents() );
                }
            }
        });

        JButton eventUp = (JButton) contextEditor.find( "up-event" );
        eventUp.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                if ( ! animation.getEvents().isEmpty() ){
                    int index1 = eventList.getSelectedIndex()-1 < 0 ? 0 : eventList.getSelectedIndex() - 1;
                    int index2 = eventList.getSelectedIndex();
                    animation.swapEvents( index1, index2 );
                    eventList.setListData( animation.getEvents() );
                    eventList.setSelectedIndex( index1 );
                    eventList.ensureIndexIsVisible( index1 );
                }
            }
        });

        JButton eventDown = (JButton) contextEditor.find( "down-event" );
        eventDown.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                if ( ! animation.getEvents().isEmpty() ){
                    int index1 = eventList.getSelectedIndex()+1 > animation.getEvents().size() ? animation.getEvents().size() : eventList.getSelectedIndex() + 1;
                    int index2 = eventList.getSelectedIndex();
                    animation.swapEvents( index1, index2 );
                    eventList.setListData( animation.getEvents() );
                    eventList.setSelectedIndex( index1 );
                    eventList.ensureIndexIsVisible( index1 );
                }
            }
        });

        return (JComponent) contextEditor.getRootComponent();
    }

    private JPanel makeOnionSkinningPanel(final Animation animation){
        final SwingEngine optionsEngine = new SwingEngine("animator/onion-options.xml");
        final JSlider skins = (JSlider) optionsEngine.find("skins");
        final JLabel many = (JLabel) optionsEngine.find("how-many");
        skins.setValue(animation.getOnionSkins());

        skins.addChangeListener(new ChangeListener(){
            private String descriptive(int what){
                if (what < 0){
                    return "Showing " + what + " past frames";
                }
                if (what == 0){
                    return "Showing no frames";
                }
                if (what > 0){
                    return "Showing " + what + " future frames";
                }
                return "WTF";
            }

            public void stateChanged(ChangeEvent change){
                many.setText(descriptive(skins.getValue()));
                animation.setOnionSkins(skins.getValue());
                animation.forceRedraw();
            }
        });

        final JCheckBox onionSkinning = (JCheckBox) optionsEngine.find("onion-skinning");
        onionSkinning.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                animation.setOnionSkinning(onionSkinning.isSelected());
                animation.forceRedraw();
            }
        });

        final JRadioButton front = (JRadioButton) optionsEngine.find("front");
        final JRadioButton back = (JRadioButton) optionsEngine.find("back");
        front.setActionCommand("front");
        back.setActionCommand("back");
        
        AbstractAction change = new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                if (event.getActionCommand().equals("front")){
                    animation.setOnionSkinFront(true);
                } else {
                    animation.setOnionSkinFront(false);
                }
                
                animation.forceRedraw();
            }
        };

        front.addActionListener(change);
        back.addActionListener(change);

        return (JPanel) optionsEngine.getRootComponent();
    }

    private JPanel makeAdjustOffsetsPanel(final Animation animation){
        final SwingEngine optionsEngine = new SwingEngine("animator/adjust-offsets.xml");

        class OffsetAction extends AbstractAction {
            public OffsetAction(Lambda1 doOffset){
                this.doOffset = doOffset;
            }

            Lambda1 doOffset;

            public List<OffsetEvent> getOffsets(Vector<AnimationEvent> events){
                List<OffsetEvent> offsets = new ArrayList<OffsetEvent>();
                for (AnimationEvent event: events){
                    /* instanceof is justified here because we can treat
                     * events like an ADT
                     */
                    if (event instanceof OffsetEvent){
                        offsets.add((OffsetEvent) event);
                    }
                }
                return offsets;
            }

            private void updateOffsets(){
                for (OffsetEvent offset: getOffsets(animation.getEvents())){
                    try{
                        doOffset.invoke(offset);
                    } catch (Exception e){
                        System.out.println(e);
                    }
                }
            }

            public void actionPerformed(ActionEvent event){
                updateOffsets();
                animation.applyEvents();
                animation.forceRedraw();
            }
        };

        JButton left = (JButton) optionsEngine.find("left");
        JButton right = (JButton) optionsEngine.find("right");
        JButton up = (JButton) optionsEngine.find("up");
        JButton down = (JButton) optionsEngine.find("down");

        left.addActionListener(new OffsetAction(new Lambda1(){
            public Object invoke(Object o){
                OffsetEvent offset = (OffsetEvent) o;
                offset.setX(offset.getX() - 1);
                return null;
            }
        }));

        right.addActionListener(new OffsetAction(new Lambda1(){
            public Object invoke(Object o){
                OffsetEvent offset = (OffsetEvent) o;
                offset.setX(offset.getX() + 1);
                return null;
            }
        }));

        up.addActionListener(new OffsetAction(new Lambda1(){
            public Object invoke(Object o){
                OffsetEvent offset = (OffsetEvent) o;
                offset.setY(offset.getY() - 1);
                return null;
            }
        }));

        down.addActionListener(new OffsetAction(new Lambda1(){
            public Object invoke(Object o){
                OffsetEvent offset = (OffsetEvent) o;
                offset.setY(offset.getY() + 1);
                return null;
            }
        }));

        return (JPanel) optionsEngine.getRootComponent();
    }
                        
    private void showAdjustOffsetsDialog(final Animation animation, final Vector<AnimationEvent> events){
        final JDialog dialog = new JDialog();
        dialog.setTitle("Adjust offsets");
        dialog.setSize(new Dimension(200, 200));
                
        final SwingEngine optionsEngine = new SwingEngine("animator/adjust-offsets.xml");

        class OffsetAction extends AbstractAction {
            public OffsetAction(Lambda1 doOffset){
                this.doOffset = doOffset;
            }

            Lambda1 doOffset;

            public List<OffsetEvent> getOffsets(Vector<AnimationEvent> events){
                List<OffsetEvent> offsets = new ArrayList<OffsetEvent>();
                for (AnimationEvent event: events){
                    /* instanceof is justified here because we can treat
                     * events like an ADT
                     */
                    if (event instanceof OffsetEvent){
                        offsets.add((OffsetEvent) event);
                    }
                }
                return offsets;
            }

            private void updateOffsets(){
                for (OffsetEvent offset: getOffsets(events)){
                    try{
                        doOffset.invoke(offset);
                    } catch (Exception e){
                        System.out.println(e);
                    }
                }
            }

            public void actionPerformed(ActionEvent event){
                updateOffsets();
                animation.applyEvents();
                animation.forceRedraw();
            }
        };

        JButton left = (JButton) optionsEngine.find("left");
        JButton right = (JButton) optionsEngine.find("right");
        JButton up = (JButton) optionsEngine.find("up");
        JButton down = (JButton) optionsEngine.find("down");

        left.addActionListener(new OffsetAction(new Lambda1(){
            public Object invoke(Object o){
                OffsetEvent offset = (OffsetEvent) o;
                offset.setX(offset.getX() - 1);
                return null;
            }
        }));

        right.addActionListener(new OffsetAction(new Lambda1(){
            public Object invoke(Object o){
                OffsetEvent offset = (OffsetEvent) o;
                offset.setX(offset.getX() + 1);
                return null;
            }
        }));

        up.addActionListener(new OffsetAction(new Lambda1(){
            public Object invoke(Object o){
                OffsetEvent offset = (OffsetEvent) o;
                offset.setY(offset.getY() - 1);
                return null;
            }
        }));

        down.addActionListener(new OffsetAction(new Lambda1(){
            public Object invoke(Object o){
                OffsetEvent offset = (OffsetEvent) o;
                offset.setY(offset.getY() + 1);
                return null;
            }
        }));

        /*
        JButton apply = (JButton) optionsEngine.find("apply");
        JButton cancel = (JButton) optionsEngine.find("apply");

        apply.addActionListener(new AbstractAction(){
            public List<OffsetEvent> getOffsets(Vector<AnimationEvent> events){
                List<OffsetEvent> offsets = new ArrayList<OffsetEvent>();
                for (AnimationEvent event: events){
                    if (event instanceof OffsetEvent){
                        offsets.add((OffsetEvent) event);
                    }
                }
                return offsets;
            }

            public void actionPerformed(ActionEvent event){
                int xValue = ((Integer) x.getValue()).intValue();
                int yValue = ((Integer) y.getValue()).intValue();
                for (OffsetEvent offset: getOffsets(events)){
                    offset.setX(offset.getX() + xValue);
                    offset.setY(offset.getY() + yValue);
                }
            }
        });

        cancel.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                dialog.hide();
            }
        });
        */

        dialog.getContentPane().add((JPanel) optionsEngine.getRootComponent());
        dialog.show();

    }

    private void adjustSlider(JSlider slider, int much){
        slider.setValue(slider.getValue() + much);
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
