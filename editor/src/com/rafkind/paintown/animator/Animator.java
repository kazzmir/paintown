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

import com.rafkind.paintown.animator.AnimationEditor;
import com.rafkind.paintown.animator.IQueue;
import com.rafkind.paintown.animator.Player;

public class Animator extends JFrame {

	private static File dataPath = new File( "data" );
	private static int CURRENT_TAB = 0;

	public Animator() throws Exception {
		super( "Paintown Animator" );
		this.setSize( 900, 500 );

		JMenuBar menuBar = new JMenuBar();
		JMenu menuProgram = new JMenu( "Program" );
		JMenuItem quit = new JMenuItem( "Quit" );
		JMenuItem data = new JMenuItem( "Data path" );
		menuProgram.add( data );
		menuProgram.add( quit );
		menuBar.add( menuProgram );
		JMenu menuAnimation = new JMenu( "Animation" );
		menuBar.add( menuAnimation );
		final Lambda0 closeHook = new Lambda0(){
		  public Object invoke(){
		  System.exit( 0 );
		  return null;
		  }
		  };
		JMenuItem newAnimation = new JMenuItem( "New Animation" );
		menuAnimation.add( newAnimation );
		JMenuItem loadAnimation = new JMenuItem( "Open Animation" );
		menuAnimation.add( loadAnimation );
		JMenuItem saveAnimation = new JMenuItem( "Save Animation" );
		menuAnimation.add( saveAnimation );
		JMenuItem saveAnimationAs = new JMenuItem( "Save Animation As" );
		menuAnimation.add( saveAnimationAs );
		JMenuItem closeAnimation = new JMenuItem( "Close Animation" );
		menuAnimation.add( closeAnimation );

		menuProgram.setMnemonic( KeyEvent.VK_P );
		data.setMnemonic( KeyEvent.VK_D );
		quit.setMnemonic( KeyEvent.VK_Q );
		newAnimation.setMnemonic( KeyEvent.VK_N );
		menuAnimation.setMnemonic( KeyEvent.VK_I );
		saveAnimation.setMnemonic( KeyEvent.VK_S );
		saveAnimationAs.setMnemonic( KeyEvent.VK_A );
		loadAnimation.setMnemonic( KeyEvent.VK_O );
		closeAnimation.setMnemonic( KeyEvent.VK_C );

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
		
		// tabs
		final JTabbedPane pane = new JTabbedPane();
		getContentPane().add(pane);
		
		pane.addChangeListener( new ChangeListener()
		{
			public void stateChanged(ChangeEvent changeEvent)
			{
				JTabbedPane sourceTabbedPane = (JTabbedPane) changeEvent.getSource();
				int index = sourceTabbedPane.getSelectedIndex();
				CURRENT_TAB = index;
			}
     		});
		
		newAnimation.addActionListener( new AbstractAction()
		{
			public void actionPerformed( ActionEvent event )
			{
				
				AnimationEditor newTab = new AnimationEditor();
				
				Player tempPlayer = new Player();
				
				newTab.add(tempPlayer.getEditor());
				
				pane.add( Integer.toString(pane.getTabCount()), newTab );
				
				pane.setSelectedIndex(pane.getTabCount()-1);
				
				CURRENT_TAB = pane.getSelectedIndex();
			}
		});
		
		closeAnimation.addActionListener( new AbstractAction()
		{
			public void actionPerformed( ActionEvent event)
			{
				pane.remove(CURRENT_TAB);
			}
		});
	}

	public static File dataPath( File f ){
		return new File( getDataPath().getPath() + "/" + f.getPath() );
	}

	public static String dataPath( String s ){
		return getDataPath().getPath() + "/" + s;
	}

	private static File getDataPath(){
		return dataPath;
	}

	private static void setDataPath( File f ){
		dataPath = f;
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
