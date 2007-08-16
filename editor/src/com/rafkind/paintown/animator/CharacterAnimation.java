package com.rafkind.paintown.animator;

import java.util.*;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.event.*;
import javax.swing.Timer;
import java.io.*;

import org.swixml.SwingEngine;

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


public class CharacterAnimation
{
	private DrawArea area;
	// private DrawArea externalArea;
	private SwingEngine animEditor;
	private SwingEngine contextEditor;
	private SwingEngine controlEditor;
	private JPanel context;
	private JPanel canvas;
	private JPanel other;
	private JPanel controls;
	private JTextField nameField;
	private JComboBox typeCombo;
	private JList keyList;
	private JComboBox keySelect;
	private JButton keyAdd;
	private JButton keyRemove;
	private JButton keyUp;
	private JButton keyDown;
	private JSpinner rangeSpinner;
	private JTextField basedirField;
	private JButton basedirButton;
	private JList eventList;
	private JComboBox eventSelect;
	private JButton eventAdd;
	private JButton eventEdit;
	private JButton eventRemove;
	private JButton eventUp;
	private JButton eventDown;
	
	private JButton displayToken;
	
	// Name
	// private String name = "";
	
	// Type if special
	private String type = "";
	
	// key sequence (String)
	// private Vector keys = new Vector();
	
	// Range
	// private int range;
	
	// Base dir
	private String baseDirectory = "";
	
	// Events
	// private Vector events = new Vector();
	
	private Timer timer;
	
	/*
	public void setName(String n)
	{
		name = n;
	}
	*/
	
	/*
	public String getName()
	{
		return name;
	}
	*/
	
	public void setType(String t)
	{
		type = t;
	}
	
	public String getType()
	{
		return type;
	}
	
	/*
	public void addKey(String key)
	{
		keys.addElement(key);
	}
	
	public Vector getKeys()
	{
		return keys;
	}
	
	public void clearKeys()
	{
		keys.clear();
	}
	*/
	
	/*
	public void setRange(int r)
	{
		range = r;
	}
	
	public int getRange()
	{
		return range;
	}
	*/
	
	public void fixBaseDirectory()
	{
		DrawState.setCurrentDirList(Animator.dataPath(baseDirectory));
	}
	
	public void setBaseDirectory(String b)
	{
		baseDirectory = b;
	}
	
	public String getBaseDirectory()
	{
		return baseDirectory;
	}
	
	/*
	public void addEvent(AnimationEvent event)
	{
		events.addElement(event);
	}
	
	public void removeEvent(AnimationEvent event)
	{
		events.removeElement(event);
	}
	
	public Vector getEvents()
	{
		return events;
	}
	*/
	
	private void startAnimation(){
		/*
		area.registerEvents(events);
		timer.start();
		*/
	}
	
	private void stopAnimation(){
		/*
		area.resetEvents();
		timer.stop();
		*/
	}
	
	/*
	public void startAnim(DrawArea drawarea){
		externalArea = drawarea;
		externalArea.registerEvents(events);
	}
	*/
	
	/*
	public void stopAnim(){
		externalArea.resetEvents();
	}
	*/
	
	/*
	public Token getToken(){
		Token token = new Token();
		token.addToken( new Token( "anim" ) );
		
		token.addToken(new String[]{"name", "\"" + name + "\""});
		if(!type.equals("none"))token.addToken(new String[]{"type", type});
		if(!keys.isEmpty())
		{
			Token keyToken = new Token( "keys" );
			keyToken.addToken( new Token( "keys"));
			Iterator kItor = keys.iterator();
			while(kItor.hasNext())
			{
				String key = (String)kItor.next();
				keyToken.addToken(new Token(key));
			}
			token.addToken(keyToken);
		}
		if(range!=0)token.addToken(new String[]{"range", Integer.toString(range)});
		if(!baseDirectory.equals(""))token.addToken(new String[]{"basedir", baseDirectory});
		Iterator fItor = events.iterator();
		while(fItor.hasNext())
		{
			AnimationEvent event = (AnimationEvent)fItor.next();
			token.addToken(event.getToken());
		}
		
		return token;
	}
	*/
	
	public SpecialPanel getEditor()
	{	
		return new SpecialPanel((JPanel)animEditor.getRootComponent(),nameField);
	}
	
	public DrawArea getDrawArea()
	{
		return area;
	}
	
	public CharacterAnimation( final Animation animation ){
		type = "none";
		
		animEditor = new SwingEngine( "animator/base.xml" );
		
		contextEditor = new SwingEngine ( "animator/animation.xml");
		
		controlEditor = new SwingEngine( "animator/controls.xml" );
		
		context = (JPanel) animEditor.find( "context" );
		
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
		
		typeCombo = (JComboBox) contextEditor.find( "type" );
		typeCombo.addItem(new String("none"));
		typeCombo.addItem(new String("attack"));
		typeCombo.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				type = (String)typeCombo.getSelectedItem();
			}
		});
		
		keyList = (JList) contextEditor.find( "keys");
		keySelect = (JComboBox) contextEditor.find( "key-select" );

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

		keyAdd = (JButton) contextEditor.find( "add-key" );
		keyAdd.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				animation.addKey( (String) keySelect.getSelectedItem() );
				keyList.setListData( animation.getKeys() );
			}
		});
		keyRemove = (JButton) contextEditor.find( "remove-key" );
		keyRemove.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				if ( ! animation.getKeys().isEmpty() ){
					animation.removeKey( keyList.getSelectedIndex() );
					keyList.setListData( animation.getKeys() );
				}
			}
		});
		keyUp = (JButton) contextEditor.find( "up-key" );
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
		keyDown = (JButton) contextEditor.find( "down-key" );
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
		
		rangeSpinner = (JSpinner) contextEditor.find( "range" );
		rangeSpinner.setValue( new Integer( animation.getRange() ) );
		rangeSpinner.addChangeListener( new ChangeListener(){
			public void stateChanged(ChangeEvent changeEvent){
				animation.setRange( ((Integer)rangeSpinner.getValue()).intValue() );
			}
		});
		
		basedirField = (JTextField) contextEditor.find( "basedir" );
		basedirButton = (JButton) contextEditor.find( "change-basedir" );
		basedirButton.addActionListener( new AbstractAction(){
				public void actionPerformed( ActionEvent event ){
					RelativeFileChooser chooser = Animator.getNewFileChooser();
					int ret = chooser.open();
					if ( ret == RelativeFileChooser.OK ){
						final String path = chooser.getPath();
						basedirField.setText( path );
						baseDirectory = path;
						DrawState.setCurrentDirList(Animator.dataPath(baseDirectory));
					}
				}
			});
		
		eventList = (JList) contextEditor.find( "events");

		eventList.setListData( animation.getEvents() );
		
		eventList.addListSelectionListener(new ListSelectionListener()
		{
			public void valueChanged(ListSelectionEvent e){
				// TODO: fixme
				/*
				if(!events.isEmpty() && !timer.isRunning()){
					((AnimationEvent)eventList.getSelectedValue()).interact(area);
					area.repaint();
				}
				*/
			}
		});
		
		eventList.addMouseListener( new MouseAdapter() {
			public void mouseClicked(MouseEvent e) {
				if (e.getClickCount() == 2) {
					int index = eventList.locationToIndex(e.getPoint());
					AnimationEvent temp = (AnimationEvent) animation.getEvents().elementAt(index);
					JDialog dialog = temp.getEditor();
					if(dialog != null){
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
				setForeground(isSelected ? Color.white : Color.black);
				return this;
			}
		});
		
		// Need to add events to this combobox from event factory
		// EventFactory.init();
		eventSelect = (JComboBox) contextEditor.find( "event-select" );
		for ( Iterator it = EventFactory.getNames().iterator(); it.hasNext(); ){
			String event = (String) it.next();
			eventSelect.addItem( event );
		}
		
		eventAdd = (JButton) contextEditor.find( "add-event" );
		eventAdd.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				AnimationEvent temp = EventFactory.getEvent((String)eventSelect.getSelectedItem());
				JDialog dialog = temp.getEditor();
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
		
		eventEdit = (JButton) contextEditor.find( "edit-event" );
		eventEdit.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				if( ! animation.getEvents().isEmpty()){
					AnimationEvent temp = (AnimationEvent) animation.getEvents().elementAt( eventList.getSelectedIndex() );
					JDialog dialog = temp.getEditor();
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
		
		eventRemove = (JButton) contextEditor.find( "remove-event" );
		eventRemove.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				if ( ! animation.getEvents().isEmpty() ){
					animation.removeEvent( eventList.getSelectedIndex() );
					eventList.setListData( animation.getEvents() );
				}
			}
		});
		
		eventUp = (JButton) contextEditor.find( "up-event" );
		eventUp.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				if ( ! animation.getEvents().isEmpty() ){
					animation.swapEvents( eventList.getSelectedIndex()-1 < 0 ? 0 : eventList.getSelectedIndex() - 1, eventList.getSelectedIndex() );
					eventList.setListData( animation.getEvents() );
				}
			}
		});
		
		eventDown = (JButton) contextEditor.find( "down-event" );
		eventDown.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				if ( ! animation.getEvents().isEmpty() ){
					animation.swapEvents( eventList.getSelectedIndex()+1 > animation.getEvents().size() ? animation.getEvents().size() : eventList.getSelectedIndex() + 1, eventList.getSelectedIndex() );
					eventList.setListData( animation.getEvents() );
				}
			}
		});
		
		controls = (JPanel) animEditor.find( "controls" );
		
		displayToken = (JButton) controlEditor.find( "token" );
		
		displayToken.addActionListener( new AbstractAction()
		{
			public void actionPerformed( ActionEvent event )
			{
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
		
		controls.add((JComponent)controlEditor.getRootComponent());
		
		canvas = (JPanel) animEditor.find( "canvas" );
		area = new DrawArea();
		canvas.add(area);

		area.animate( animation );
		
		other = (JPanel) animEditor.find( "other" );
		
		context.add((JComponent)contextEditor.getRootComponent());
		
		timer = new Timer(1,new AbstractAction() {
			public void actionPerformed(ActionEvent e) {	
				eventList.setSelectedValue(area.getCurrentEvent(),true);
			}
		});
	}
	
	/*
	public void loadData(Token token) throws LoadException{
		if ( ! token.getName().equals( "anim" ) ){
			throw new LoadException( "Starting token is not 'anim'" );
		}
		
		Token nameToken = token.findToken( "name" );
		if ( nameToken != null )
		{
			nameField.setText(nameToken.readString(0));
			name = nameToken.readString(0);
		}
		
		Token typeToken = token.findToken( "type" );
		if ( typeToken != null )
		{
			for(int i=0; i < typeCombo.getItemCount();++i)
			{
				if(((String)typeCombo.getItemAt(i)).equals(typeToken.readString(0)))
				{
					typeCombo.setSelectedIndex(i);
					type = nameToken.readString(0);
					break;
				}
			}
			
		}
		
		Token keyToken = token.findToken( "keys" );
		if ( keyToken != null ){
			try{
				for(int i = 0; ; i += 1 ){
					String temp = keyToken.readString(i);
					if(temp != null){
						keys.addElement(temp);
					} else {
						break;
					}
				}
			} catch(Exception e) {
				e.printStackTrace();
			}

			keyList.setListData( keys );
		}
		
		Token rangeToken = token.findToken( "range" );
		if ( rangeToken != null ){
			rangeSpinner.setValue(new Integer(rangeToken.readInt(0)));
			range = rangeToken.readInt(0);
		}
		
		Token basedirToken = token.findToken( "basedir" );
		if ( basedirToken != null )
		{
			basedirField.setText(basedirToken.readString(0));
			baseDirectory = basedirToken.readString(0);
			DrawState.setCurrentDirList(Animator.dataPath(baseDirectory));
		}
		
		for ( Iterator it = token.getTokens().iterator(); it.hasNext(); ){
			Token t = (Token) it.next();
			AnimationEvent ae = EventFactory.getEvent(t.getName());
			if(ae != null)
			{
				ae.loadToken(t);
				events.addElement(ae);
			}
		}
		eventList.setListData(events);
	}
	*/
}
