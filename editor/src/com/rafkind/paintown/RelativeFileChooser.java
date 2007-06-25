package com.rafkind.paintown;

import javax.swing.*;
import javax.swing.event.*;
import java.awt.event.*;

import java.util.List;
import java.util.ArrayList;
import java.io.File;

import org.swixml.SwingEngine;

public class RelativeFileChooser extends JDialog {

	public static final int OK = 0;
	public static final int CANCEL = 1;

	private List paths;
	private int option;

	public RelativeFileChooser( JFrame frame, File start ){
		super( frame, "Choose a file", true );
		this.setSize( 300, 300 );

		SwingEngine engine = new SwingEngine( "relative.xml" );
		JPanel panel = (JPanel) engine.getRootComponent();
		this.getContentPane().add( panel );

		this.addWindowListener( new WindowAdapter(){
			public void windowClosing( WindowEvent e ){
				setOption( CANCEL );
			}
		});

		paths = new ArrayList();
		paths.add( start.getName() );
		option = 0;
	}

	private void setOption( int i ){
		this.option = i;
	}

	private int getOption(){
		return this.option;
	}

	public int open(){
		this.show();
		return getOption();
	}

	public String getPath(){
		return "";
	}

}
