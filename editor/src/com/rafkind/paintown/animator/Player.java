package com.rafkind.paintown.animator;

import java.util.*;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.event.*;
import java.io.*;

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
    private Animation currentAnimation;

    public SpecialPanel getEditor(){
        return new SpecialPanel((JPanel)playerEditor.getRootComponent(), nameField, character );
    }

    public Player(final NewAnimator animator, final CharacterStats character){
        this.character = character;

        playerEditor = new SwingEngine( "animator/base.xml" );

        final SwingEngine contextEditor = new SwingEngine("animator/context.xml");
        final SwingEngine controlEditor = new SwingEngine("animator/controls.xml");

        //debugSwixml(playerEditor);
        //debugSwixml(contextEditor);

        final JSplitPane split = (JSplitPane) playerEditor.find("split");
        SwingUtilities.invokeLater(new Runnable(){
            public void run(){
                /*
                if ( split.getDividerLocation() != -1 ){
                    split.setDividerLocation(0.6);
                } else {
                    SwingUtilities.invokeLater(this);
                }
                */
                if (split.getWidth() != 0){
                    split.setDividerLocation(0.6);
                } else {
                    SwingUtilities.invokeLater(this);
                }
            }

        });

        final JPanel context = (JPanel) playerEditor.find( "context" );
        final JTabbedPane animations = (JTabbedPane) playerEditor.find("animations");

        final Lambda2 changeName = new Lambda2(){
            public Object invoke(Object self, Object name){
                animations.setTitleAt(animations.indexOfComponent((JComponent) self), (String) name);
                return this;
            }
        };

        final JButton addAnimation = (JButton) playerEditor.find("add-animation");
        addAnimation.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                Animation animation = new Animation();
                character.addAnimation(animation);
                new Thread(animation).start();
                JComponent tab = new CharacterAnimation(character, animation, changeName);
                animations.add("New animation", tab);
                animations.setSelectedComponent(tab);
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
                    tab.getAnimation().kill();
                    character.removeAnimation(tab.getAnimation());
                    animations.remove(tab);
                }
            }
        });


        for (Animation animation : character.getAnimations()){
            animations.add(animation.getName(), new CharacterAnimation(character, animation, changeName));
	}

        /*
        // final JPanel canvas = (JPanel) playerEditor.find( "canvas" );

        final DrawArea _drawArea = new DrawArea(new Lambda0(){
            public Object invoke(){
                / * yes, this should be null unless something better
                 * can be put in the popup place
                 * /
                return null;
            }
        });

        GridBagConstraints constraints = new GridBagConstraints();
        constraints.gridx = 0;
        constraints.gridy = 0;
        constraints.weightx = 1;
        constraints.weighty = 1;
        constraints.fill = GridBagConstraints.BOTH;
        constraints.anchor = GridBagConstraints.NORTHWEST;

        // canvas.add(_drawArea, constraints);

        final JLabel scaleNum = (JLabel) playerEditor.find( "scale-num" );
        scaleNum.setText( "Scale: " + _drawArea.getScale() );
        final JSlider scale = (JSlider) playerEditor.find( "scale" );
        scale.setValue( (int)(_drawArea.getScale() * 5.0) );
        scale.addChangeListener( new ChangeListener(){
            public void stateChanged( ChangeEvent e ){
                _drawArea.setScale( scale.getValue() / 5.0 );
                scaleNum.setText( "Scale: " + _drawArea.getScale() );
            }
        });
        */

        nameField = (JTextField) contextEditor.find( "name" );

        nameField.setText( character.getName() );

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

        final JSpinner healthSpinner = (JSpinner) contextEditor.find( "health" );
        healthSpinner.setValue( new Integer( character.getHealth() ) );

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

        final JTextField origMapField = (JTextField) contextEditor.find( "original-map" );
        origMapField.setText( character.getOriginalMap() );

        final JButton origMapButton = (JButton) contextEditor.find( "change-origmap" );

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

        /*
        final Lambda1 editAnimation = new Lambda1(){
            public Object invoke( Object i ){
                int index = ((Integer) i).intValue();
                Animation temp = character.getAnimation( index );
                CharacterAnimation edit = new CharacterAnimation(character, temp, changeName);
                animator.addNewTab(edit.getEditor(), temp.getName());
                return null;
            }
        };
        */

        /*
        final JList animList = (JList) contextEditor.find( "anims");
        animList.setListData( character.getAnimations() );

        animList.addMouseListener( new MouseAdapter() {
            public void mouseClicked(MouseEvent e) {
                if (e.getClickCount() == 2) {
                    int index = animList.locationToIndex(e.getPoint());
                    editAnimation.invoke_( new Integer( index ) );
                }
            }
        });

        animList.setCellRenderer(new DefaultListCellRenderer() {
            public Component getListCellRendererComponent(
                JList list,
                Object value,
                int index,
                boolean isSelected,
                boolean cellHasFocus){

                setText(((Animation)value).getName());
                setBackground(isSelected ? Color.gray : Color.white);
                setForeground(isSelected ? Color.white : Color.black);
                return this;
                }
        });

        final JButton addAnimButton = (JButton) contextEditor.find( "add-anim" );

        addAnimButton.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                Animation ani = new Animation();
                new Thread( ani ).start();
                character.addAnimation( ani );
                editAnimation.invoke_( new Integer( character.getAnimations().size() - 1 ) );

                character.updateAnimationListeners();

                animList.setListData( character.getAnimations() );

                //System.out.println(getToken().toString());
            }
        });

        final JButton editAnimButton = (JButton) contextEditor.find( "edit-anim" );

        editAnimButton.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                editAnimation.invoke_( new Integer( animList.getSelectedIndex() ) );
            }
        });

        final JButton removeAnimButton = (JButton) contextEditor.find( "remove-anim" );

        removeAnimButton.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                character.removeAnimation( animList.getSelectedIndex() );
                animList.setListData( character.getAnimations() );
            }
        });
        */

        /*
        final JPanel controls = (JPanel) playerEditor.find( "controls" );

        final JButton displayToken = (JButton) controlEditor.find( "token" );

        displayToken.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                final JDialog tempDiag = new JDialog();
                tempDiag.setSize(400,400);
                final JTextArea tempText = new JTextArea();
                final JScrollPane tempPane = new JScrollPane(tempText);
                tempDiag.add(tempPane);
                tempText.setText( character.getToken().toString() );
                tempDiag.show();
            }
        });

        final JButton stopAnim = (JButton) controlEditor.find( "stop" );
        stopAnim.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                _drawArea.unanimate();
                if ( currentAnimation != null ){
                    currentAnimation.stopRunning();
                }
            }
        });

        final JButton playAnim = (JButton) controlEditor.find( "play" );
        playAnim.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                if( animList.getSelectedValue() != null ){
                    if ( currentAnimation != null ){
                        currentAnimation.stopRunning();
                    }
                    currentAnimation = (Animation) animList.getSelectedValue();
                    _drawArea.animate( currentAnimation );
                    currentAnimation.startRunning();
                }
            }
        });

        controls.add((JComponent)controlEditor.getRootComponent());
        */

        context.add((JComponent)contextEditor.getRootComponent());
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
        System.out.println( "Debugging swixml" );
        for ( Iterator it = all.entrySet().iterator(); it.hasNext(); ){
            Map.Entry entry = (Map.Entry) it.next();
            System.out.println( "Id: " + entry.getKey() + " = " + entry.getValue() );
        }
    }
}
