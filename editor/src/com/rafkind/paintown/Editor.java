package com.rafkind.paintown;

import java.util.*;
import java.awt.event.*;
import javax.swing.*;

import org.swixml.SwingEngine;

public class Editor extends JFrame {

	public Editor(){
		this.setSize( 1000, 600 );

		JMenuBar menuBar = new JMenuBar();
		JMenu menuProgram = new JMenu( "Program" );
		menuBar.add( menuProgram );
		JMenuItem quit = new JMenuItem( "Quit" );
		menuProgram.add( quit );
		final Lambda0 closeHook = new Lambda0(){
			public Object invoke(){
				System.exit( 0 );
				return null;
			}
		};

		quit.addActionListener( new ActionListener(){
			public void actionPerformed( ActionEvent event ){
				closeHook.invoke_();
			}
		});

		SwingEngine engine = new SwingEngine( "main.xml" );
		this.getContentPane().add( (JPanel) engine.getRootComponent() );

		JList list = (JList) engine.find( "files" );
		Vector v = new Vector();
		v.add( "foo" );
		v.add( "foo" );
		v.add( "foo" );
		v.add( "foo" );
		v.add( "foo" );
		v.add( "foo" );
		v.add( "foo" );
		v.add( "foo" );
		v.add( "foo" );
		v.add( "foo" );
		v.add( "foo" );
		v.add( "foo" );
		v.add( "foo" );
		v.add( "foo" );
		list.setListData( v );

		this.setJMenuBar( menuBar );
		this.addWindowListener( new CloseHook( closeHook ) );
	}

	public static void main( String[] args ){

		final Editor editor = new Editor();
		SwingUtilities.invokeLater(
			new Runnable(){
				public void run(){
					editor.setVisible( true );
				}
			}
		);
	}
}
