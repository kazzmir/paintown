package com.rafkind.paintown.animator;

import java.util.*;
import java.awt.*;
import java.awt.image.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import java.io.*;

import java.util.List;

import com.rafkind.paintown.exception.LoadException;

import javax.swing.filechooser.FileFilter;

import org.swixml.SwingEngine;

import java.awt.event.*;
import javax.swing.*;

import com.rafkind.paintown.*;

import com.rafkind.paintown.animator.IQueue;
import com.rafkind.paintown.animator.Player;
import com.rafkind.paintown.animator.SpecialPanel;
import com.rafkind.paintown.animator.DrawState;

public class Animator extends JFrame {

	private static File dataPath = new File( "data" );
	private static int CURRENT_TAB = 0;
	private static Animator _animator;
	
	private static JTabbedPane pane = new JTabbedPane();

	public Animator() throws Exception {
		super( "Paintown Animator" );
		_animator = this;
		this.setSize( (int)Toolkit.getDefaultToolkit().getScreenSize().getWidth() - 50, (int)Toolkit.getDefaultToolkit().getScreenSize().getHeight() - 50 );

		JMenuBar menuBar = new JMenuBar();
		
		JMenu menuProgram = new JMenu( "Program" );
		JMenuItem quit = new JMenuItem( "Quit" );
		JMenuItem data = new JMenuItem( "Data path" );
		JMenuItem closeTab = new JMenuItem( "Close Tab" );
		menuProgram.add( data );
		menuProgram.add( closeTab );
		menuProgram.add( quit );
		menuBar.add( menuProgram );
		
		JMenu menuCharacter = new JMenu( "Character" );
		menuBar.add( menuCharacter );
		
		JMenuItem newCharacter = new JMenuItem( "New Character" );
		menuCharacter.add( newCharacter );
		JMenuItem loadCharacter = new JMenuItem( "Open Character" );
		menuCharacter.add( loadCharacter );
		JMenuItem saveCharacter = new JMenuItem( "Save Character" );
		menuCharacter.add( saveCharacter );
		JMenuItem saveCharacterAs = new JMenuItem( "Save Character As" );
		menuCharacter.add( saveCharacterAs );
		
		final Lambda0 closeHook = new Lambda0(){
		  public Object invoke(){
		  System.exit( 0 );
		  return null;
		  }
		  };

		menuProgram.setMnemonic( KeyEvent.VK_P );
		data.setMnemonic( KeyEvent.VK_D );
		closeTab.setMnemonic( KeyEvent.VK_C );
		closeTab.setAccelerator(KeyStroke.getKeyStroke(
                            KeyEvent.VK_W, Event.CTRL_MASK));
		quit.setMnemonic( KeyEvent.VK_Q );
		quit.setAccelerator(KeyStroke.getKeyStroke(
                            KeyEvent.VK_Q, Event.CTRL_MASK));
		newCharacter.setMnemonic( KeyEvent.VK_N );
		newCharacter.setAccelerator(KeyStroke.getKeyStroke(
                            KeyEvent.VK_N, Event.CTRL_MASK));
		menuCharacter.setMnemonic( KeyEvent.VK_H );
		saveCharacter.setMnemonic( KeyEvent.VK_S );
		saveCharacter.setAccelerator(KeyStroke.getKeyStroke(
                            KeyEvent.VK_S, Event.CTRL_MASK));
		saveCharacterAs.setMnemonic( KeyEvent.VK_A );
		saveCharacterAs.setAccelerator(KeyStroke.getKeyStroke(
                            KeyEvent.VK_A, Event.CTRL_MASK));
		loadCharacter.setMnemonic( KeyEvent.VK_O );
		loadCharacter.setAccelerator(KeyStroke.getKeyStroke(
                            KeyEvent.VK_O, Event.CTRL_MASK));


		this.setJMenuBar( menuBar );
		this.addWindowListener( new CloseHook( closeHook ) );		

		quit.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent evt) {
				System.exit(0);
			}
		});

		data.addActionListener( new ActionListener(){
			public void actionPerformed( ActionEvent event ){
				/* just a container for an object */
				class ObjectBox {
					private Object internal;
					public ObjectBox(){
					}

					public void set( Object o ){
						internal = o;
					}

					public Object get(){
						return internal;
					}
				}
				final SwingEngine engine = new SwingEngine( "data-path.xml" );
				final JTextField path = (JTextField) engine.find( "path" );
				final ObjectBox box = new ObjectBox();
				box.set( getDataPath() );
				path.setText( getDataPath().getPath() );
				final JButton change = (JButton) engine.find( "change" );
				change.addActionListener( new AbstractAction(){
					public void actionPerformed( ActionEvent event ){
						JFileChooser chooser = new JFileChooser( new File( "." ) );	
						chooser.setFileSelectionMode( JFileChooser.DIRECTORIES_ONLY );
						int returnVal = chooser.showOpenDialog( Animator.this );
						if ( returnVal == JFileChooser.APPROVE_OPTION ){
							final File newPath = chooser.getSelectedFile();
							path.setText( newPath.getPath() );
							box.set( newPath );
						}
					}
				});
				final JButton save = (JButton) engine.find( "save" );
				final JButton cancel = (JButton) engine.find( "cancel" );
				final JDialog dialog = new JDialog( Animator.this, "Paintown data path" );
				save.addActionListener( new AbstractAction(){
					public void actionPerformed( ActionEvent event ){
						setDataPath( (File) box.get() );
						dialog.setVisible( false );
					}
				});

				cancel.addActionListener( new AbstractAction(){
					public void actionPerformed( ActionEvent event ){
						dialog.setVisible( false );
					}
				});

				JPanel panel = (JPanel) engine.getRootComponent();
				dialog.getContentPane().add( panel );
				dialog.setSize( 300, 300 );
				dialog.setVisible( true );
			}
		});
		
		// Lets add our tabbedPane and some actions
		
		getContentPane().add(pane);
		
		closeTab.addActionListener( new AbstractAction()
		{
			public void actionPerformed( ActionEvent event)
			{
				pane.remove(CURRENT_TAB);
			}
		});
		
		pane.addChangeListener( new ChangeListener()
		{
			public void stateChanged(ChangeEvent changeEvent)
			{
				JTabbedPane sourceTabbedPane = (JTabbedPane) changeEvent.getSource();
				int index = sourceTabbedPane.getSelectedIndex();
				CURRENT_TAB = index;
			}
     		});
		
		newCharacter.addActionListener( new AbstractAction()
		{
			public void actionPerformed( ActionEvent event )
			{
				// This will need to change to a factory or something
				Player tempPlayer = new Player(Animator.this);
				
				addNewTab(tempPlayer.getEditor(), "New Character");
			}
		});
		
		loadCharacter.addActionListener( new AbstractAction()
		{
			public void actionPerformed( ActionEvent event )
			{
				JFileChooser chooser = new JFileChooser( new File( "." ) );	
				chooser.setFileFilter( new FileFilter(){
					public boolean accept( File f ){
						return f.isDirectory() || f.getName().endsWith( ".txt" );
					}

					public String getDescription(){
						return "Character files";
					}
				});

				int returnVal = chooser.showOpenDialog( Animator.this );
				if ( returnVal == JFileChooser.APPROVE_OPTION ){
					final File f = chooser.getSelectedFile();
					try{
						Player tempPlayer = new Player(Animator.this);
						tempPlayer.loadData(f);
						addNewTab(tempPlayer.getEditor(), tempPlayer.getName());
					} catch ( LoadException le ){
						//showError( "Could not load " + f.getName() );
						System.out.println( "Could not load " + f.getName() );
						le.printStackTrace();
					}
				}	
			}
		});
		
		saveCharacter.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				if ( pane.getSelectedComponent() != null ){
					Player player = ((SpecialPanel)pane.getSelectedComponent()).getPlayer();
					if(player!= null)
					{
						File file = player.getPath();
						if ( file == null ){
							file = userSelectFile();
						}
						/* write the text to a file */
						if ( file != null ){
							try{
								player.saveData(file);
							} catch ( Exception e ){
								e.printStackTrace();
								//showError( "Could not save " + file + " because " + e.getMessage() );
							}
						}
					}
				}
			}
		});

		saveCharacterAs.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				if ( pane.getSelectedComponent() != null ){
					Player player = ((SpecialPanel)pane.getSelectedComponent()).getPlayer();
					if(player!=null)
					{
						File file = userSelectFile();
						/* write the text to a file */
						if ( file != null ){
							try{
								player.saveData(file);
							} catch ( Exception e ){
								e.printStackTrace();
								//showError( "Could not save " + file + " because " + e.getMessage() );
							}
						}
					}
				}
			}
		});
	}
	
	private File userSelectFile(){
		JFileChooser chooser = new JFileChooser( new File( "." ) );
		int returnVal = chooser.showOpenDialog( Animator.this );
		if ( returnVal == JFileChooser.APPROVE_OPTION ){
			return chooser.getSelectedFile();
		} else {
			return null;
		}
	}

	public static File dataPath( File f ){
		return new File( getDataPath().getPath() + "/" + f.getPath() );
	}

	public static String dataPath( String s ){
		return getDataPath().getPath() + "/" + s;
	}

	public static File getDataPath(){
		return dataPath;
	}

	private static void setDataPath( File f ){
		dataPath = f;
	}
	
	public static RelativeFileChooser getNewFileChooser()
	{
		return new RelativeFileChooser(_animator, getDataPath());
	}
	
	public void addNewTab( SpecialPanel panel , String name)
	{
		pane.add( name , panel );
		
		pane.setSelectedIndex(pane.getTabCount()-1);
		
		CURRENT_TAB = pane.getSelectedIndex();
		
		final SpecialPanel tempPanel = (SpecialPanel)pane.getComponentAt(CURRENT_TAB);
		
		panel.getTextBox().getDocument().addDocumentListener(new DocumentListener()
		{
			public void changedUpdate(DocumentEvent e)
			{
				pane.setTitleAt(pane.indexOfComponent(tempPanel),tempPanel.getTextBox().getText());
			}
			public void insertUpdate(DocumentEvent e)
			{
				pane.setTitleAt(pane.indexOfComponent(tempPanel),tempPanel.getTextBox().getText());
			}
			public void removeUpdate(DocumentEvent e)
			{
				pane.setTitleAt(pane.indexOfComponent(tempPanel),tempPanel.getTextBox().getText());
			}
		});
	}

	public static void main(String [] args) throws Exception {
		final Animator animator = new Animator();
		SwingUtilities.invokeLater( new Runnable(){
			public void run(){
				animator.setVisible( true );
			}
		});
	}
}
