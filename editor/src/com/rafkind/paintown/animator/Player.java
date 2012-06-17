package com.rafkind.paintown.animator;

import java.util.*;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.event.*;
import java.io.*;

import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.DataFlavor;
import javax.activation.DataHandler;

import org.swixml.SwingEngine;
import javax.swing.filechooser.FileFilter;

import com.rafkind.paintown.animator.CharacterStats;
import com.rafkind.paintown.animator.DrawArea;
import com.rafkind.paintown.exception.*;
import com.rafkind.paintown.*;

public final class Player{

    private JTextField nameField;
    private SwingEngine playerEditor;
    private CharacterStats character;

    private static class ObjectBox<Data>{
        public ObjectBox(){}
        public synchronized void set(Data x){ obj = x; }
        public synchronized Data get(){ return obj; }
        private Data obj;
    }

    public SpecialPanel getEditor(){
        return new SpecialPanel((JPanel)playerEditor.getRootComponent(), nameField, character );
    }

    public Player(final NewAnimator animator, final CharacterStats character){
        this.character = character;

        playerEditor = new SwingEngine("animator/base.xml");

        final SwingEngine contextEditor = new SwingEngine("animator/context.xml");
        // final SwingEngine controlEditor = new SwingEngine("animator/controls.xml");

        final JComponent animationPane = (JComponent) playerEditor.find("animation-pane");
        final JComponent propertiesPane = (JComponent) playerEditor.find("properties-pane");
        final JComponent comboPane = (JComponent) playerEditor.find("combo-pane");

        final JTabbedPane all = (JTabbedPane) playerEditor.find("all");

        all.setTitleAt(all.indexOfComponent(animationPane), "Animations");
        all.setTitleAt(all.indexOfComponent(propertiesPane), "Properties");
        all.setTitleAt(all.indexOfComponent(comboPane), "Combo Viewer");
        
        //debugSwixml(playerEditor);
        //debugSwixml(contextEditor);

        final JPanel context = (JPanel) playerEditor.find("context");
        final JTabbedPane animations = (JTabbedPane) playerEditor.find("animations");

        final Lambda2 changeName = new Lambda2(){
            public Object invoke(Object self, Object name){
                animations.setTitleAt(animations.indexOfComponent((JComponent) self), (String) name);
                return this;
            }
        };

        final Detacher detacher = new Detacher(){
            JFrame lastFrame;
            public void destroyFrame(JComponent object){
                if (lastFrame != null){
                    lastFrame.setVisible(false);
                    lastFrame = null;
                }
            }

            public JFrame detach(final JComponent object, String name){
                animations.remove(object);
                JFrame frame = new JFrame(name);
                lastFrame = frame;
                frame.setSize(600, 600);
                frame.getContentPane().add(object);
                frame.setVisible(true);

                frame.addWindowListener(new WindowAdapter(){
                    public void windowClosing(WindowEvent event){
                        destroyFrame(object);
                    }
                });

                return frame;
            }

            public void attach(JComponent object, final String name){
                animations.add(object, name);
                animations.setSelectedComponent(object);
                destroyFrame(object);
            }
        };

        final JButton addAnimation = (JButton) playerEditor.find("add-animation");
        addAnimation.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                Animation animation = new Animation();
                character.addAnimation(animation);
                new Thread(animation).start();
                JComponent tab = new CharacterAnimation(character, animation, changeName, detacher);
                animations.add("New animation", tab);
                animations.setSelectedComponent(tab);
            }
        });

        final JButton copyAnimation = (JButton) playerEditor.find("copy-animation");
        copyAnimation.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                CharacterAnimation current = (CharacterAnimation) animations.getSelectedComponent();
                if (current != null){
                    Animation animation = new Animation(current.getAnimation());
                    character.addAnimation(animation);
                    new Thread(animation).start();
                    JComponent tab = new CharacterAnimation(character, animation, changeName, detacher);
                    animations.add(animation.getName(), tab);
                    animations.setSelectedComponent(tab);
                }
            }
        });

        final JButton removeAnimButton = (JButton) playerEditor.find("remove-animation");
        removeAnimButton.addActionListener( new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                if (okToRemoveAnimation(character)){
                    /* TODO: if the player is unsaved then ask them if they
                     * really mean to remove the animation
                     */
                    CharacterAnimation tab = (CharacterAnimation) animations.getSelectedComponent();
                    if (tab != null){
                        tab.getAnimation().kill();
                        character.removeAnimation(tab.getAnimation());
                        animations.remove(tab);
                    }
                }
            }
        });


        for (Animation animation : character.getAnimations()){
            animations.add(animation.getName(), new CharacterAnimation(character, animation, changeName, detacher));
        }

        nameField = (JTextField) contextEditor.find("name");
        nameField.setText(character.getName());

        nameField.getDocument().addDocumentListener(new DocumentListener(){
            public void changedUpdate(DocumentEvent e){
                character.setName( nameField.getText() );
            }

            public void insertUpdate(DocumentEvent e){
                character.setName( nameField.getText() );
            }

            public void removeUpdate(DocumentEvent e){
                character.setName( nameField.getText() );
            }
        });

        final JSpinner remap = (JSpinner) contextEditor.find( "remap" );

        class RemapSpinnerModel implements SpinnerModel {
            private java.util.List listeners;
            int index;

            public RemapSpinnerModel(){
                listeners = new ArrayList();
                index = 0;
            }

            public void addChangeListener( ChangeListener l ){
                listeners.add( l );
            }

            public Object getNextValue(){
                if ( index < character.getMaxMaps() ){
                    return new Integer( index + 1 );
                }
                return new Integer( index );
            }

            public Object getPreviousValue(){
                if ( index > 0 ){
                    return new Integer( index - 1 );
                }
                return new Integer( index );
            }

            public Object getValue(){
                return new Integer( index );
            }

            public void removeChangeListener( ChangeListener l ){
                listeners.remove( l );
            }

            public void setValue( Object value ){
                index = ((Integer) value).intValue();
                ChangeEvent event = new ChangeEvent( this );
                for ( Iterator it = listeners.iterator(); it.hasNext(); ){
                    ChangeListener change = (ChangeListener) it.next();
                    change.stateChanged( event );
                }
            }
        }

        remap.setModel( new RemapSpinnerModel() );

        remap.setValue( new Integer( 0 ) );
        remap.addChangeListener( new ChangeListener(){
            public void stateChanged(ChangeEvent changeEvent){
                character.setMap( ((Integer) remap.getValue()).intValue() - 1 );
            }
        });

        final JSpinner scaleSpinner = (JSpinner) contextEditor.find("scale");
        scaleSpinner.setModel(new SpinnerNumberModel(character.getSpriteScale(), 0.001, 100, 0.1));
        scaleSpinner.setValue(new Double(character.getSpriteScale()));
        scaleSpinner.addChangeListener(new ChangeListener(){
            public void stateChanged(ChangeEvent changeEvent){
                character.setSpriteScale(((Double) scaleSpinner.getValue()).doubleValue());
            }
        });

        final JSpinner healthSpinner = (JSpinner) contextEditor.find("health");
        healthSpinner.setValue(new Integer(character.getHealth()));

        healthSpinner.addChangeListener( new ChangeListener(){
            public void stateChanged(ChangeEvent changeEvent){
                character.setHealth( ((Integer)healthSpinner.getValue()).intValue() );
            }
        });

        final JPanel jumpSpinner = (JPanel) contextEditor.find( "jump-velocity" );

        final JSpinner jumpSpinner2 = new JSpinner(new SpinnerNumberModel(0, -1000, 1000, 0.1));
        jumpSpinner2.setValue( new Double( character.getJumpVelocity() ) );

        jumpSpinner2.addChangeListener( new ChangeListener(){
            public void stateChanged(ChangeEvent changeEvent){
                character.setJumpVelocity( ((Double)jumpSpinner2.getValue()).doubleValue() );
            }
        });

        jumpSpinner.add(jumpSpinner2);

        final JPanel speedSpinner = (JPanel) contextEditor.find( "speed" );

        final JSpinner speedSpinner2 = new JSpinner(new SpinnerNumberModel(0, -1000, 1000, 0.1));
        speedSpinner2.setValue( new Double( character.getSpeed() ) );

        speedSpinner.add(speedSpinner2);

        speedSpinner2.addChangeListener( new ChangeListener(){
            public void stateChanged(ChangeEvent changeEvent){
                character.setSpeed( ((Double)speedSpinner2.getValue()).doubleValue() );
            }
        });

        final JSpinner shadowSpinner = (JSpinner) contextEditor.find( "shadow" );
        shadowSpinner.setValue( new Integer( character.getShadow() ) );

        shadowSpinner.addChangeListener( new ChangeListener(){
            public void stateChanged(ChangeEvent changeEvent){
                character.setShadow( ((Integer)shadowSpinner.getValue()).intValue() );
            }
        });

        final JTextField deathSoundField = (JTextField) contextEditor.find( "die-sound" );
        deathSoundField.setText( character.getDieSound() );

        final JButton deathSoundButton = (JButton) contextEditor.find( "change-die-sound" );

        deathSoundButton.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                RelativeFileChooser chooser = NewAnimator.getNewFileChooser();
                int ret = chooser.open();
                if ( ret == RelativeFileChooser.OK ){
                    final String path = chooser.getPath();
                    deathSoundField.setText( path );
                    character.setDieSound( path );
                }
            }
        });

        final JTextField hitSoundField = (JTextField) contextEditor.find( "hit-sound" );
        hitSoundField.setText( character.getHitSound() );
        JButton hitSoundButton = (JButton) contextEditor.find( "change-hit-sound" );
        hitSoundButton.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                RelativeFileChooser chooser = NewAnimator.getNewFileChooser();
                int ret = chooser.open();
                if ( ret == RelativeFileChooser.OK ){
                    final String path = chooser.getPath();
                    hitSoundField.setText( path );
                    character.setHitSound( path );
                }
            }
        });

        final JTextField landingSoundField = (JTextField) contextEditor.find( "land-sound" );
        landingSoundField.setText( character.getLanded() );

        final JButton landingSoundButton = (JButton) contextEditor.find( "change-land-sound" );

        landingSoundButton.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                RelativeFileChooser chooser = NewAnimator.getNewFileChooser();
                int ret = chooser.open();
                if ( ret == RelativeFileChooser.OK ){
                    final String path = chooser.getPath();
                    landingSoundField.setText( path );
                    character.setLanded( path );
                }
            }
        });

        final JTextField introField = (JTextField) contextEditor.find("intro");
        introField.setText(character.getIntro());

        final JButton introButton = (JButton) contextEditor.find("change-intro");

        introButton.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                RelativeFileChooser chooser = NewAnimator.getNewFileChooser();
                int ret = chooser.open();
                if ( ret == RelativeFileChooser.OK ){
                    final String path = chooser.getPath();
                    introField.setText(path);
                    character.setIntro(path);
                }
            }
        });


        final JTextField iconField = (JTextField) contextEditor.find( "icon" );
        iconField.setText( character.getIcon() );

        final JButton iconButton = (JButton) contextEditor.find( "change-icon" );

        iconButton.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                RelativeFileChooser chooser = NewAnimator.getNewFileChooser();
                int ret = chooser.open();
                if ( ret == RelativeFileChooser.OK ){
                    final String path = chooser.getPath();
                    iconField.setText( path );
                    character.setIcon( path );
                }
            }
        });

        final JTextField origMapField = (JTextField) contextEditor.find("original-map");
        origMapField.setText(character.getOriginalMap());

        final JButton origMapButton = (JButton) contextEditor.find("change-origmap");

        origMapButton.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                RelativeFileChooser chooser = NewAnimator.getNewFileChooser();
                int ret = chooser.open();
                if ( ret == RelativeFileChooser.OK ){
                    final String path = chooser.getPath();
                    origMapField.setText( path );
                    character.setOriginalMap( path );
                }
            }
        });

        final JList remapList = (JList) contextEditor.find( "remaps" );
        remapList.setListData( character.getRemaps() );

        final JButton addRemapButton = (JButton) contextEditor.find( "add-remap" );

        addRemapButton.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                RelativeFileChooser chooser = NewAnimator.getNewFileChooser();
                int ret = chooser.open();
                if ( ret == RelativeFileChooser.OK ){
                    final String path = chooser.getPath();
                    character.addMap( path );
                    remapList.setListData( character.getRemaps() );
                }
            }
        });

        final JButton removeRemapButton = (JButton) contextEditor.find( "remove-remap" );

        removeRemapButton.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                String temp = character.getMap( remapList.getSelectedIndex() );
                character.removeMap( temp );
                remapList.setListData( character.getRemaps() );
            }
        });

        context.add((JComponent) contextEditor.getRootComponent(), filledConstraints());

        comboPane.add(makeComboPane(character), filledConstraints());
    }

    private JPanel makeComboPane(final AnimatedObject object){
        final SwingEngine context = new SwingEngine("animator/combo.xml");

        final DrawArea area = new DrawArea(object.getDrawProperties(), new Lambda0(){
            public Object invoke(){
                return null;
            }
        });

        final JPanel canvas = (JPanel) context.find("canvas");
        canvas.add(area, filledConstraints());

        final JComboBox animations = (JComboBox) context.find("select");
        for (Animation animation: object.getAnimations()){
            animations.addItem(animation);
        }

        final Vector<Animation> animationSequenceData = new Vector();
        final JList animationSequence = (JList) context.find("animations");

        final Lambda1 updateAnimations = new Lambda1(){
            public Object invoke(Object objectSelf){
                /* TODO: should we remove animations from the sequence
                 * that were removed from the object? probably yes..
                 */
                animations.removeAllItems();
                for (Animation animation: object.getAnimations()){
                    animations.addItem(animation);
                }
                return null;
            }
        };

        object.addAnimationUpdate(updateAnimations);

        final JButton addAnimation = (JButton) context.find("add");
        final JButton removeAnimation = (JButton) context.find("remove");

        final ObjectBox<Integer> currentAnimation = new ObjectBox<Integer>();
        currentAnimation.set(0);

        animationSequence.setCellRenderer(new DefaultListCellRenderer(){
            public Component getListCellRendererComponent(
                JList list,
                Object value,
                int index,
                boolean isSelected,
                boolean cellHasFocus){

                setText(((Animation)value).getName());
                setBackground(isSelected ? Color.gray : Color.white);
                if (currentAnimation.get() == index){
                    setForeground(Color.blue);
                } else {
                    // setForeground(isSelected ? Color.white : Color.black);
                    setForeground(Color.black);
                }
                return this;
            }
        });

        final Lambda1 nextAnimation = new Lambda1(){
            public Object invoke(Object self){
                int index = currentAnimation.get();
                if (index < animationSequenceData.size()){
                    animationSequenceData.get(index).stopRunning();
                }

                index = (index + 1 + animationSequenceData.size()) % animationSequenceData.size();
                currentAnimation.set(index);
                if (index < animationSequenceData.size()){
                    Animation animation = animationSequenceData.get(index);
                    animation.startRunning();
                    area.animate(animation);
                    animationSequence.repaint();
                }

                return null;
            }
        };

        animationSequence.setDragEnabled(true);
        animationSequence.setDropMode(DropMode.ON);
        animationSequence.setTransferHandler(new TransferHandler(){
            private int toRemove = -1;

            public boolean canImport(TransferHandler.TransferSupport support){
                return true;
            }

            protected void exportDone(JComponent source, Transferable data, int action){
                if (action == TransferHandler.MOVE){
                    animationSequenceData.remove(toRemove);
                    animationSequence.setListData(animationSequenceData);
                }
            }

            public int getSourceActions(JComponent component){
                return TransferHandler.MOVE;
            }

            protected Transferable createTransferable(JComponent component){
                return new Transferable(){
                    final Object index = animationSequence.getSelectedValue();
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
            }

            public boolean importData(TransferSupport transfer) {
                if (!canImport(transfer)) {
                    return false;
                }

                try{
                    JList.DropLocation location = (JList.DropLocation) transfer.getDropLocation();
                    toRemove = animationSequence.getSelectedIndex();
                    int index = location.getIndex();
                    if (index < toRemove){
                        toRemove += 1;
                    }
                    Animation animation = (Animation) transfer.getTransferable().getTransferData(new DataFlavor(DataFlavor.javaJVMLocalObjectMimeType));
                    animationSequenceData.add(index, animation);
                    animationSequence.setSelectedIndex(index);
                } catch (ClassNotFoundException exception){
                    System.err.println(exception);
                } catch (java.awt.datatransfer.UnsupportedFlavorException exception){
                    System.err.println(exception);
                } catch (IOException exception){
                    System.err.println(exception);
                }

                return true;
            }

        });

        addAnimation.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                Animation animation = (Animation) animations.getSelectedItem();
                animationSequenceData.add(animation);
                animationSequence.setListData(animationSequenceData);
                animation.addLoopNotifier(nextAnimation);
            }
        });

        removeAnimation.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                if (animationSequence.getSelectedIndex() != -1){
                    animationSequenceData.remove(animationSequence.getSelectedIndex());
                    animationSequence.setListData(animationSequenceData);
                }
            }
        });

        final JButton play = (JButton) context.find("play");
        final JButton stop = (JButton) context.find("stop");

        play.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                int index = currentAnimation.get();
                if (index >= animationSequenceData.size()){
                    index = 0;
                }
                if (index < animationSequenceData.size()){
                    Animation animation = animationSequenceData.get(index);
                    animation.startRunning();
                    area.animate(animation);
                }
            }
        });

        stop.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                int index = currentAnimation.get();
                if (index < animationSequenceData.size()){
                    Animation animation = animationSequenceData.get(index);
                    animation.stopRunning();
                }
            }
        });

        final JButton nextFrame = (JButton) context.find("next-frame");
        final JButton previousFrame = (JButton) context.find("previous-frame");

        nextFrame.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                if (currentAnimation.get() < animationSequenceData.size()){
                    Animation animation = animationSequenceData.get(currentAnimation.get());
                    animation.nextFrame();
                    animation.forceRedraw();
                }
            }
        });

        previousFrame.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                if (currentAnimation.get() < animationSequenceData.size()){
                    Animation animation = animationSequenceData.get(currentAnimation.get());
                    animation.previousFrame();
                    animation.forceRedraw();
                }
            }
        });


        final JComboBox tools = (JComboBox) context.find("tools");
        final JPanel toolPane = (JPanel) context.find("tool-area");
        
        final String chooseNone = "None";
        final String chooseBackground = "Background Color";
        final String chooseGrid = "Grid";
        final String chooseSpeedAndScale = "Speed and Scale";

        tools.addItem(chooseNone);
        tools.addItem(chooseSpeedAndScale);
        tools.addItem(chooseBackground);
        tools.addItem(chooseGrid);

        tools.addActionListener(new AbstractAction(){
            /* TODO: If there are too many tools then create them lazily so we
             * don't spend too much time creating the animation pane on startup.
             */
            final JPanel toolNone = new JPanel();
            final JPanel toolBackground = Tools.makeBackgroundTool(object, area);
            final JPanel toolGrid = Tools.makeGridTool(area);
            final JPanel toolSpeedAndScale = makeSpeedAndScale(animationSequenceData, area);

            private JPanel getTool(String name){
                if (name.equals(chooseNone)){
                    return toolNone;
                }
                if (name.equals(chooseSpeedAndScale)){
                    return toolSpeedAndScale;
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

        return (JPanel) context.getRootComponent();
    }
            
    private JPanel makeSpeedAndScale(final Vector<Animation> animations, final DrawArea area){
        final SwingEngine context = new SwingEngine("animator/tool-speed-scale.xml");
        final JLabel animationSpeed = (JLabel) context.find("speed-num");
        final JSlider speed = (JSlider) context.find("speed");
        final double speedNumerator = 20.0;
        if (animations.size() > 0){
            animationSpeed.setText("Animation speed: " + (int)(100 * animations.get(0).getAnimationSpeed()) + "%");
            speed.setValue((int) (speedNumerator / animations.get(0).getAnimationSpeed()));
        } else {
            speed.setValue(20);
            animationSpeed.setText("Animation speed: 100%");
        }
        speed.addChangeListener( new ChangeListener(){
            public void stateChanged(ChangeEvent e){
                for (Animation animation: animations){
                    animation.setAnimationSpeed(speedNumerator / speed.getValue());
                }
                animationSpeed.setText("Animation speed: " + (int)(100 * speed.getValue() / speedNumerator) + "%");
            }
        });

        final JButton speedIncrease = (JButton) context.find("speed:increase");
        final JButton speedDecrease = (JButton) context.find("speed:decrease");

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

        final JLabel scaleNum = (JLabel) context.find( "scale-num" );
        scaleNum.setText("Scale: " + (int)(100 * area.getScale()) + "%");
        final JSlider scale = (JSlider) context.find( "scale" );
        scale.setValue( (int)(area.getScale() * 5.0) );
        scale.addChangeListener( new ChangeListener(){
            public void stateChanged( ChangeEvent e ){
                area.setScale(scale.getValue() / 5.0);
                scaleNum.setText("Scale: " + (int)(100 * area.getScale()) + "%");
            }
        });

        final JButton scaleIncrease = (JButton) context.find("scale:increase");
        final JButton scaleDecrease = (JButton) context.find("scale:decrease");

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

        return (JPanel) context.getRootComponent();
    }

    private GridBagConstraints filledConstraints(){
        GridBagConstraints constraints = new GridBagConstraints();
        constraints.gridx = 0;
        constraints.gridy = 0;
        constraints.weightx = 1;
        constraints.weighty = 1;
        constraints.fill = GridBagConstraints.BOTH;
        constraints.anchor = GridBagConstraints.NORTHWEST;
        return constraints;
    }

    private boolean okToRemoveAnimation(CharacterStats character){
        return !isUnsaved(character) ||
               (isUnsaved(character) && acceptRemoval());
    }

    /* true if the character has any unsaved changes */
    private boolean isUnsaved(CharacterStats character){
        /* TODO */
        return false;
    }

    /* popup a dialog box asking if the user really wants to do this */
    private boolean acceptRemoval(){
        /* TODO */
        return false;
    }

    private void debugSwixml( SwingEngine engine ){
        Map all = engine.getIdMap();
        System.out.println("Debugging swixml");
        for ( Iterator it = all.entrySet().iterator(); it.hasNext(); ){
            Map.Entry entry = (Map.Entry) it.next();
            System.out.println( "Id: " + entry.getKey() + " = " + entry.getValue() );
        }
    }
    
    /* TODO: move this to a utility class */
    private void adjustSlider(JSlider slider, int much){
        slider.setValue(slider.getValue() + much);
    }
}
