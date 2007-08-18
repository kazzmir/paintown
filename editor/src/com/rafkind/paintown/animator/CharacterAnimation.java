package com.rafkind.paintown.animator;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.event.*;
import javax.swing.Timer;
import java.io.*;

import org.swixml.SwingEngine;

import com.rafkind.paintown.Lambda1;
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

import java.util.List;
import java.util.Iterator;
import java.util.ArrayList;

public class CharacterAnimation{
	
	private SwingEngine animEditor;
	private JTextField nameField;
	private CharacterStats saveCharacter;
	
	public SpecialPanel getEditor(){	
		return new SpecialPanel((JPanel)animEditor.getRootComponent(),nameField, saveCharacter );
	}

	private boolean rightClick( MouseEvent event ){
		return event.getButton() == MouseEvent.BUTTON3;
	}
	
	public CharacterAnimation( final CharacterStats character, final Animation animation ){
		animEditor = new SwingEngine( "animator/base.xml" );

		this.saveCharacter = character;
		
		SwingEngine contextEditor = new SwingEngine ( "animator/animation.xml");
		
		SwingEngine controlEditor = new SwingEngine( "animator/controls.xml" );
		
		JPanel context = (JPanel) animEditor.find( "context" );
		
		nameField = (JTextField) contextEditor.find( "name" );
		
		nameField.setText( animation.getName() );
		
		nameField.getDocument().addDocumentListener(new DocumentListener(){
			public void changedUpdate(DocumentEvent e){
				animation.setName( nameField.getText() );
			}

			public void insertUpdate(DocumentEvent e){
				animation.setName( nameField.getText() );
			}

			public void removeUpdate(DocumentEvent e){
				animation.setName( nameField.getText() );
			}
		});
		
		final JComboBox typeCombo = (JComboBox) contextEditor.find( "type" );
		typeCombo.addItem(new String("none"));
		typeCombo.addItem(new String("attack"));
		typeCombo.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				animation.setType( (String) typeCombo.getSelectedItem() );
			}
		});
		typeCombo.setSelectedItem( animation.getType() );
		
		final JList keyList = (JList) contextEditor.find( "keys");
		final JComboBox keySelect = (JComboBox) contextEditor.find( "key-select" );

		keySelect.addItem(new String("key_idle"));
		keySelect.addItem(new String("key_up"));
		keySelect.addItem(new String("key_down"));
		keySelect.addItem(new String("key_back"));
		keySelect.addItem(new String("key_forward"));
		keySelect.addItem(new String("key_upperback"));
		keySelect.addItem(new String("key_upperforward"));
		keySelect.addItem(new String("key_downback"));
		keySelect.addItem(new String("key_downforward"));
		keySelect.addItem(new String("key_jump"));
		keySelect.addItem(new String("key_block"));
		keySelect.addItem(new String("key_attack1"));
		keySelect.addItem(new String("key_attack2"));
		keySelect.addItem(new String("key_attack3"));
		keySelect.addItem(new String("key_attack4"));
		keySelect.addItem(new String("key_attack5"));
		keySelect.addItem(new String("key_attack6"));

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
				animations = getAnimations( character );
				selected = null;

				character.addAnimationUpdate( new Lambda1(){
					public Object invoke( Object o ){
						CharacterStats who = (CharacterStats) o;
						animations = new ArrayList();
						animations = getAnimations( who );
						
						updateAll();
						return null;
					}
				});
			}

			private List getAnimations( CharacterStats who ){
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
		
		final JList eventList = (JList) contextEditor.find( "events");

		eventList.setListData( animation.getEvents() );

		class ObjectBox{
			public ObjectBox(){}
			public synchronized void set( Object x ){ obj = x; }
			public synchronized Object get(){ return obj; }
			private Object obj;
		}
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
		
		eventList.addMouseListener( new MouseAdapter() {
			public void mouseClicked(MouseEvent e) {
				if ( rightClick( e ) || e.getClickCount() == 2 ){
					int index = eventList.locationToIndex(e.getPoint());
					AnimationEvent event = (AnimationEvent) animation.getEvents().elementAt(index);
					JDialog dialog = event.getEditor( animation );
					if ( dialog != null ){
						dialog.addWindowStateListener(new WindowStateListener(){
							public void windowStateChanged(WindowEvent e){
								/* should use a list update event here */
								eventList.setListData( animation.getEvents() );
							}
						});
						dialog.show();
					}
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
		for ( Iterator it = EventFactory.getNames().iterator(); it.hasNext(); ){
			String event = (String) it.next();
			eventSelect.addItem( event );
		}
		
		JButton eventAdd = (JButton) contextEditor.find( "add-event" );
		eventAdd.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				AnimationEvent temp = EventFactory.getEvent((String)eventSelect.getSelectedItem());
				JDialog dialog = temp.getEditor( animation );
				if ( dialog != null ){
					dialog.addWindowStateListener(new WindowStateListener(){
						public void windowStateChanged(WindowEvent e){
							eventList.setListData( animation.getEvents() );
						}
					});
					dialog.show();
				}
				animation.addEvent( temp );
				eventList.setListData( animation.getEvents() );
			}
		});
		
		JButton eventEdit = (JButton) contextEditor.find( "edit-event" );
		eventEdit.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				if( ! animation.getEvents().isEmpty()){
					AnimationEvent temp = (AnimationEvent) animation.getEvents().elementAt( eventList.getSelectedIndex() );
					JDialog dialog = temp.getEditor( animation );
					if ( dialog != null ){
						dialog.addWindowStateListener(new WindowStateListener(){
							public void windowStateChanged(WindowEvent e){
								eventList.setListData( animation.getEvents() );
							}
						});
						dialog.show();
					}
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
		
		JButton stopAnim = (JButton) controlEditor.find( "stop" );
		stopAnim.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				animation.stopRunning();
			}
		});

		JButton playAnim = (JButton) controlEditor.find( "play" );
		playAnim.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				animation.startRunning();
			}
		});

		final JLabel animationSpeed = (JLabel) contextEditor.find( "speed-num" );
		animationSpeed.setText( "Animation speed: " + animation.getAnimationSpeed() );
		final JSlider speed = (JSlider) contextEditor.find( "speed" );
		speed.setValue( (int) (20 / animation.getAnimationSpeed()) );
		speed.addChangeListener( new ChangeListener(){
			public void stateChanged( ChangeEvent e ){
				animation.setAnimationSpeed( 20.0 / speed.getValue() );
				animationSpeed.setText( "Animation speed: " + speed.getValue() / 20.0 );
			}
		});
		
		controls.add((JComponent)controlEditor.getRootComponent());
		
		JPanel canvas = (JPanel) animEditor.find( "canvas" );
		DrawArea area = new DrawArea();
		canvas.add(area);

		area.animate( animation );
		
		JPanel other = (JPanel) animEditor.find( "other" );
		
		context.add((JComponent)contextEditor.getRootComponent());
	}
}
