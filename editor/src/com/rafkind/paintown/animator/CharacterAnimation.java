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

public class CharacterAnimation
{
	private DrawArea area;
	private SwingEngine animEditor;
	private SwingEngine contextEditor;
	private JPanel context;
	private JPanel canvas;
	private JPanel other;
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
	
	// Name
	private String name = "";
	
	// Type if special
	private String type = "";
	
	// key sequence (String)
	private Vector keys = new Vector();
	
	// Range
	private int range;
	
	// Base dir
	private String baseDirectory = "";
	
	// Events
	private Vector events = new Vector();
	
	private Timer timer;
	
	public void setName(String n)
	{
		name = n;
	}
	
	public String getName()
	{
		return name;
	}
	
	public void setType(String t)
	{
		type = t;
	}
	
	public String getType()
	{
		return type;
	}
	
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
	
	public void setRange(int r)
	{
		range = r;
	}
	
	public int getRange()
	{
		return range;
	}
	
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
	
	public void startAnimation(int msecs)
	{
	 			 if(timer.isRunning())stopAnimation();
	 			 Action doAnim = new AbstractAction() {
				   Iterator itor = events.iterator();
					 public void actionPerformed(ActionEvent e) {	
	  			 
	  			 				try
	  			 				{
				  			 				AnimationEvent anim = (AnimationEvent)itor.next();
				  			 				anim.interact(area);
				  			 				area.repaint();
	  			 				}
						      catch(Exception bleh)
									{
									 								itor = events.iterator();
 								  }
           }
				 };
			
				 timer.setDelay(msecs);
				 timer.addActionListener(doAnim);
				 timer.start();
	}
	
	public void stopAnimation()
	{
	 			 timer.stop();
	 			 Vector temp = new Vector();
	 			 temp.copyInto(timer.getActionListeners());
	 			 Iterator tItor = temp.iterator();
	       while(tItor.hasNext())
					{
						   Action a = (Action)tItor.next();
						   timer.removeActionListener(a);
					}
	}
	
	public Token getToken()
	{
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
	
	public SpecialPanel getEditor()
	{	
		return new SpecialPanel((JPanel)animEditor.getRootComponent(),nameField);
	}
	
	public DrawArea getDrawArea()
	{
		return area;
	}
	
	public CharacterAnimation()
	{
		name = "New Animation";
		
		type = "none";
		
		animEditor = new SwingEngine( "animator/base.xml" );
		
		contextEditor = new SwingEngine ( "animator/animation.xml");
		
		context = (JPanel) animEditor.find( "context" );
		
		nameField = (JTextField) contextEditor.find( "name" );
		
		nameField.setText(name);
		
		nameField.getDocument().addDocumentListener(new DocumentListener()
		{
			public void changedUpdate(DocumentEvent e)
			{
				name = nameField.getText();
			}
			public void insertUpdate(DocumentEvent e)
			{
				name = nameField.getText();
			}
			public void removeUpdate(DocumentEvent e)
			{
				name = nameField.getText();
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
		keyAdd = (JButton) contextEditor.find( "add-key" );
		keyAdd.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				keys.addElement((String)keySelect.getSelectedItem());
				keyList.setListData(keys);
			}
		});
		keyRemove = (JButton) contextEditor.find( "remove-key" );
		keyRemove.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				if(keys.isEmpty())return;
				String temp = (String)keys.elementAt(keyList.getSelectedIndex());
				keys.removeElement(temp);
				keyList.setListData(keys);
			}
		});
		keyUp = (JButton) contextEditor.find( "up-key" );
		keyUp.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				if(keys.isEmpty())return;
				int index1 = keyList.getSelectedIndex()-1 < 0 ? 0 : keyList.getSelectedIndex() - 1;
				int index2 = keyList.getSelectedIndex();
				String temp1 = (String)keys.elementAt(index1);
				String temp2 = (String)keys.elementAt(index2);
				
				keys.setElementAt(temp1,index2);
				keys.setElementAt(temp2,index1);
				keyList.setListData(keys);
			}
		});
		keyDown = (JButton) contextEditor.find( "down-key" );
		keyDown.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				if(keys.isEmpty())return;
				int index1 = keyList.getSelectedIndex()+1 > keys.size() ? keys.size() : keyList.getSelectedIndex() + 1;
				int index2 = keyList.getSelectedIndex();
				String temp1 = (String)keys.elementAt(index1);
				String temp2 = (String)keys.elementAt(index2);
				
				keys.setElementAt(temp1,index2);
				keys.setElementAt(temp2,index1);
				keyList.setListData(keys);
			}
		});
		
		rangeSpinner = (JSpinner) contextEditor.find( "range" );
		rangeSpinner.addChangeListener( new ChangeListener()
		{
			public void stateChanged(ChangeEvent changeEvent)
			{
				range = ((Integer)rangeSpinner.getValue()).intValue();
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
		
		eventList.setCellRenderer(new DefaultListCellRenderer() {
			public Component getListCellRendererComponent(
				JList list,
				Object value,
				int index,
				boolean isSelected,
				boolean cellHasFocus)
			{
				setText(((AnimationEvent)value).getName());
				setBackground(isSelected ? Color.gray : Color.white);
				setForeground(isSelected ? Color.white : Color.black);
				return this;
			}
			});
		
		// Need to add events to this combobox from event factory
		EventFactory.init();
		eventSelect = (JComboBox) contextEditor.find( "event-select" );
		Iterator eItor = EventFactory.getNames().iterator();
		while(eItor.hasNext())
		{
			String event = (String)eItor.next();
			eventSelect.addItem(event);
		}
		
		eventAdd = (JButton) contextEditor.find( "add-event" );
		eventAdd.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				AnimationEvent temp = EventFactory.getEvent((String)eventSelect.getSelectedItem());
				JDialog dialog = temp.getEditor();
				if(dialog != null)
				{
						dialog.addWindowStateListener(new WindowStateListener()
						{
							public void windowStateChanged(WindowEvent e)
							{
								eventList.setListData(events);
							}
						});
						dialog.show();
				}
				events.addElement(temp);
				eventList.setListData(events);
			}
		});
		
		eventEdit = (JButton) contextEditor.find( "edit-event" );
		eventEdit.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				if(events.isEmpty())return;
				AnimationEvent temp = (AnimationEvent)events.elementAt(eventList.getSelectedIndex());
				JDialog dialog = temp.getEditor();
				if(dialog != null)
				{
						dialog.addWindowStateListener(new WindowStateListener()
						{
							public void windowStateChanged(WindowEvent e)
							{
								eventList.setListData(events);
							}
						});
						dialog.show();
				}
			}
		});
		
		eventRemove = (JButton) contextEditor.find( "remove-event" );
		eventRemove.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				if(events.isEmpty())return;
				AnimationEvent temp = (AnimationEvent)events.elementAt(eventList.getSelectedIndex());
				events.removeElement(temp);
				eventList.setListData(events);
			}
		});
		
		eventUp = (JButton) contextEditor.find( "up-event" );
		eventUp.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				if(events.isEmpty())return;
				int index1 = eventList.getSelectedIndex()-1 < 0 ? 0 : eventList.getSelectedIndex() - 1;
				int index2 = eventList.getSelectedIndex();
				AnimationEvent temp1 = (AnimationEvent)events.elementAt(index1);
				AnimationEvent temp2 = (AnimationEvent)events.elementAt(index2);
				
				events.setElementAt(temp1,index2);
				events.setElementAt(temp2,index1);
				eventList.setListData(events);
			}
		});
		
		eventDown = (JButton) contextEditor.find( "down-event" );
		eventDown.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				if(events.isEmpty())return;
				int index1 = eventList.getSelectedIndex()+1 > events.size() ? events.size() : eventList.getSelectedIndex() + 1;
				int index2 = eventList.getSelectedIndex();
				AnimationEvent temp1 = (AnimationEvent)events.elementAt(index1);
				AnimationEvent temp2 = (AnimationEvent)events.elementAt(index2);
				
				events.setElementAt(temp1,index2);
				events.setElementAt(temp2,index1);
				eventList.setListData(events);
			}
		});
		
		canvas = (JPanel) animEditor.find( "canvas" );
		area = new DrawArea();
		canvas.add(area);
		
		other = (JPanel) animEditor.find( "other" );
		
		context.add((JComponent)contextEditor.getRootComponent());
		
		timer = new Timer(0,null);
	}
	
	public void loadData(Token token) throws LoadException
	{
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
		if(keyToken != null)
		{
			try
			{
				for(int i = 0;;++i)
				{
					String temp = keyToken.readString(i);
					if(temp != null)
					{
						keys.addElement(temp);
					}
					else break;
				}
			}
			catch(Exception e)
			{
			}
			keyList.setListData(keys);
		}
		
		Token rangeToken = token.findToken( "range" );
		if ( rangeToken != null )
		{
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
	
}
