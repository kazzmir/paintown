package com.rafkind.paintown.animator;

// set tabstop=3
// set shiftwidth=3

import java.util.*;
import java.awt.*;
import java.awt.image.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import java.io.*;

import java.util.List;

import com.rafkind.paintown.exception.LoadException;

import com.rafkind.paintown.level.Level;
import com.rafkind.paintown.level.Block;
import com.rafkind.paintown.level.Thing;
import com.rafkind.paintown.level.Character;
import com.rafkind.paintown.level.Item;
import javax.swing.filechooser.FileFilter;

import org.swixml.SwingEngine;

import java.awt.event.*;
import javax.swing.*;

public class Animator extends JFrame {

	private static File dataPath = new File( "data" );

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
		JMenu menuLevel = new JMenu( "Animation" );
		menuBar.add( menuLevel );
		/*final Lambda0 closeHook = new Lambda0(){
		  public Object invoke(){
		  System.exit( 0 );
		  return null;
		  }
		  };*/
		JMenuItem newLevel = new JMenuItem( "New Animation" );
		menuLevel.add( newLevel );
		JMenuItem loadLevel = new JMenuItem( "Open Animation" );
		menuLevel.add( loadLevel );
		JMenuItem saveLevel = new JMenuItem( "Save Animation" );
		menuLevel.add( saveLevel );
		JMenuItem saveLevelAs = new JMenuItem( "Save Animation As" );
		menuLevel.add( saveLevelAs );
		JMenuItem closeLevel = new JMenuItem( "Close Animation" );
		menuLevel.add( closeLevel );

		menuProgram.setMnemonic( KeyEvent.VK_P );
		data.setMnemonic( KeyEvent.VK_D );
		quit.setMnemonic( KeyEvent.VK_Q );
		newLevel.setMnemonic( KeyEvent.VK_N );
		menuLevel.setMnemonic( KeyEvent.VK_L );
		saveLevel.setMnemonic( KeyEvent.VK_S );
		saveLevelAs.setMnemonic( KeyEvent.VK_A );
		loadLevel.setMnemonic( KeyEvent.VK_O );
		closeLevel.setMnemonic( KeyEvent.VK_C );

		this.setJMenuBar( menuBar );

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
