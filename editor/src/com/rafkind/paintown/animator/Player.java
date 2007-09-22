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
	private SwingEngine playerEditor;
	private SwingEngine contextEditor;
	private SwingEngine controlEditor;
	private JPanel context;
	private JPanel canvas;
	private JPanel controls;
	private JTextField nameField;
	private JSpinner healthSpinner;
	private JPanel jumpSpinner;
	private JSpinner jumpSpinner2;
	private JPanel speedSpinner;
	private JSpinner speedSpinner2;
	private JSpinner shadowSpinner;
	private JTextField deathSoundField;
	private JTextField hitSoundField;
	private JButton deathSoundButton;
	private JTextField landingSoundField;
	private JButton landingSoundButton;
	private JTextField iconField;
	private JButton iconButton;

	private Animation currentAnimation;
	
	private JTextField origMapField;
	private JButton origMapButton;
	private JList remapList;
	private JButton addRemapButton;
	private JButton removeRemapButton;
	
	private JList animList;
	private JButton addAnimButton;
	private JButton editAnimButton;
	private JButton removeAnimButton;
	
	
	private JButton displayToken;
	private JButton stopAnim;
	private JButton playAnim;

	private CharacterStats character;
	
	public SpecialPanel getEditor(){
		return new SpecialPanel((JPanel)playerEditor.getRootComponent(), nameField, character );
	}
	
	/*
	public DrawArea getDrawArea(){
		return _drawArea;
	}
	*/
	
	public Player( final Animator animator, final CharacterStats character ){

		this.character = character;
		
		playerEditor = new SwingEngine( "animator/base.xml" );
		
		contextEditor = new SwingEngine ( "animator/context.xml");
		
		controlEditor = new SwingEngine( "animator/controls.xml" );
		
		//debugSwixml(playerEditor);
		//debugSwixml(contextEditor);
		
		context = (JPanel) playerEditor.find( "context" );
		
		canvas = (JPanel) playerEditor.find( "canvas" );
		
		final DrawArea _drawArea = new DrawArea();

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
		
		canvas.add(_drawArea);
		
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
		remap.setValue( new Integer( -1 ) );
		remap.addChangeListener( new ChangeListener(){
			public void stateChanged(ChangeEvent changeEvent){
				character.setMap( ((Integer) remap.getValue()).intValue() );
			}
		});
		
		healthSpinner = (JSpinner) contextEditor.find( "health" );
		healthSpinner.setValue( new Integer( character.getHealth() ) );
		
		healthSpinner.addChangeListener( new ChangeListener(){
			public void stateChanged(ChangeEvent changeEvent){
				character.setHealth( ((Integer)healthSpinner.getValue()).intValue() );
			}
		});
		
		jumpSpinner = (JPanel) contextEditor.find( "jump-velocity" );
		
		jumpSpinner2 = new JSpinner(new SpinnerNumberModel(0, -1000, 1000, 0.1));
		jumpSpinner2.setValue( new Double( character.getJumpVelocity() ) );
		
		jumpSpinner2.addChangeListener( new ChangeListener(){
			public void stateChanged(ChangeEvent changeEvent){
				character.setJumpVelocity( ((Double)jumpSpinner2.getValue()).doubleValue() );
			}
		});
		
		jumpSpinner.add(jumpSpinner2);
		
		speedSpinner = (JPanel) contextEditor.find( "speed" );
		
		speedSpinner2 = new JSpinner(new SpinnerNumberModel(0, -1000, 1000, 0.1));
		speedSpinner2.setValue( new Double( character.getSpeed() ) );
		
		speedSpinner.add(speedSpinner2);
		
		speedSpinner2.addChangeListener( new ChangeListener(){
			public void stateChanged(ChangeEvent changeEvent){
				character.setSpeed( ((Double)speedSpinner2.getValue()).doubleValue() );
			}
		});
		
		shadowSpinner = (JSpinner) contextEditor.find( "shadow" );
		shadowSpinner.setValue( new Integer( character.getShadow() ) );
		
		shadowSpinner.addChangeListener( new ChangeListener(){
			public void stateChanged(ChangeEvent changeEvent){
				character.setShadow( ((Integer)shadowSpinner.getValue()).intValue() );
			}
		});
		
		deathSoundField = (JTextField) contextEditor.find( "die-sound" );
		deathSoundField.setText( character.getDieSound() );
		
		deathSoundButton = (JButton) contextEditor.find( "change-die-sound" );
		
		deathSoundButton.addActionListener( new AbstractAction(){
				public void actionPerformed( ActionEvent event ){
					RelativeFileChooser chooser = Animator.getNewFileChooser();
					int ret = chooser.open();
					if ( ret == RelativeFileChooser.OK ){
						final String path = chooser.getPath();
						deathSoundField.setText( path );
						character.setDieSound( path );
					}
				}
			});

		hitSoundField = (JTextField) contextEditor.find( "hit-sound" );
		hitSoundField.setText( character.getHitSound() );
		JButton hitSoundButton = (JButton) contextEditor.find( "change-hit-sound" );
		hitSoundButton.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				RelativeFileChooser chooser = Animator.getNewFileChooser();
				int ret = chooser.open();
				if ( ret == RelativeFileChooser.OK ){
					final String path = chooser.getPath();
					hitSoundField.setText( path );
					character.setHitSound( path );
				}
			}
		});
		
		landingSoundField = (JTextField) contextEditor.find( "land-sound" );
		landingSoundField.setText( character.getLanded() );
		
		landingSoundButton = (JButton) contextEditor.find( "change-land-sound" );
		
		landingSoundButton.addActionListener( new AbstractAction(){
				public void actionPerformed( ActionEvent event ){
					RelativeFileChooser chooser = Animator.getNewFileChooser();
					int ret = chooser.open();
					if ( ret == RelativeFileChooser.OK ){
						final String path = chooser.getPath();
						landingSoundField.setText( path );
						character.setLanded( path );
					}
				}
			});
		
		iconField = (JTextField) contextEditor.find( "icon" );
		iconField.setText( character.getIcon() );
		
		iconButton = (JButton) contextEditor.find( "change-icon" );
		
		iconButton.addActionListener( new AbstractAction(){
				public void actionPerformed( ActionEvent event ){
					RelativeFileChooser chooser = Animator.getNewFileChooser();
					int ret = chooser.open();
					if ( ret == RelativeFileChooser.OK ){
						final String path = chooser.getPath();
						iconField.setText( path );
						character.setIcon( path );
					}
				}
			});
			
		origMapField = (JTextField) contextEditor.find( "original-map" );
		origMapField.setText( character.getOriginalMap() );
		
		origMapButton = (JButton) contextEditor.find( "change-origmap" );
		
		origMapButton.addActionListener( new AbstractAction(){
				public void actionPerformed( ActionEvent event ){
					RelativeFileChooser chooser = Animator.getNewFileChooser();
					int ret = chooser.open();
					if ( ret == RelativeFileChooser.OK ){
						final String path = chooser.getPath();
						origMapField.setText( path );
						character.setOriginalMap( path );
					}
				}
			});
		
		remapList = (JList) contextEditor.find( "remaps" );
		remapList.setListData( character.getRemaps() );
		
		addRemapButton = (JButton) contextEditor.find( "add-remap" );
		
		addRemapButton.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				RelativeFileChooser chooser = Animator.getNewFileChooser();
				int ret = chooser.open();
				if ( ret == RelativeFileChooser.OK ){
					final String path = chooser.getPath();
					character.addMap( path );
					remapList.setListData( character.getRemaps() );
				}
			}
		});
		
		removeRemapButton = (JButton) contextEditor.find( "remove-remap" );
		
		removeRemapButton.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				String temp = character.getMap( remapList.getSelectedIndex() );
				character.removeMap( temp );
				remapList.setListData( character.getRemaps() );
			}
		});

		final Lambda1 editAnimation = new Lambda1(){
			public Object invoke( Object i ){
				int index = ((Integer) i).intValue();
				Animation temp = character.getAnimation( index );
				CharacterAnimation edit = new CharacterAnimation( character, temp );
				animator.addNewTab( edit.getEditor(), temp.getName());
				return null;
			}
		};
		
		animList = (JList) contextEditor.find( "anims");
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
		
		addAnimButton = (JButton) contextEditor.find( "add-anim" );
		
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
		
		editAnimButton = (JButton) contextEditor.find( "edit-anim" );
		
		editAnimButton.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				editAnimation.invoke_( new Integer( animList.getSelectedIndex() ) );
			}
		});
		
		removeAnimButton = (JButton) contextEditor.find( "remove-anim" );
		
		removeAnimButton.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				character.removeAnimation( animList.getSelectedIndex() );
				animList.setListData( character.getAnimations() );
			}
		});
		
		
		controls = (JPanel) playerEditor.find( "controls" );
		
		displayToken = (JButton) controlEditor.find( "token" );
		
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
		
		stopAnim = (JButton) controlEditor.find( "stop" );
		stopAnim.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				_drawArea.unanimate();
				if ( currentAnimation != null ){
					currentAnimation.stopRunning();
				}
			}
		});

		playAnim = (JButton) controlEditor.find( "play" );
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
		
		context.add((JComponent)contextEditor.getRootComponent());
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
