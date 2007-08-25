package com.rafkind.paintown;

import javax.swing.*;
import javax.swing.event.*;
import java.awt.event.*;

import java.util.List;
import java.util.Iterator;
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
		// paths.add( start );
		paths.add( new File( "." ) );
		option = OK;

		final JTextField path = (JTextField) engine.find( "path" );
		path.setText( getPath() );

		class FileList implements ListModel{
			List listeners = new ArrayList();
			List files = new ArrayList();

			public FileList( File start ){
				setFile( start );
			}

			public void setFile( File file ){
				files = new ArrayList();
				if ( file.isDirectory() ){
					File[] all = file.listFiles();
					files.add( new File( ".." ) );
					for ( int i = 0; i < all.length; i++ ){
						files.add( all[ i ] );
					}
				}

				ListDataEvent event = new ListDataEvent( this, ListDataEvent.CONTENTS_CHANGED, 0, 999999 );
				for ( Iterator it = listeners.iterator(); it.hasNext(); ){
					ListDataListener l = (ListDataListener) it.next();
					l.contentsChanged( event );
				}
			}

			public void removeListDataListener( ListDataListener l ){
				listeners.remove( l );
			}

			public void addListDataListener( ListDataListener l ){
				listeners.add( l );
			}

			public Object getElementAt( int index ){
				return files.get( index );
			}

			public int getSize(){
				return files.size();
			}
		};

		final FileList list = new FileList( start );

		final JList files = (JList) engine.find( "files" );
		files.setModel( list );

		files.getInputMap().put( KeyStroke.getKeyStroke( KeyEvent.VK_ENTER, 0 ), "select" );

		files.getActionMap().put( "select", new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				File file = (File) files.getSelectedValue();
				paths.add( file );
				list.setFile( file );
				path.setText( getPath() );
				if ( ! file.isDirectory() ){
					RelativeFileChooser.this.setVisible( false );
				}

				if ( list.getSize() > 0 ){
					files.setSelectedIndex( 0 );
					files.ensureIndexIsVisible( 0 );
				}
			}
		});

		files.addMouseListener( new MouseAdapter() {
			public void mouseClicked( MouseEvent clicked ){
				if ( clicked.getClickCount() == 2 ){
					Action action = files.getActionMap().get( "select" );
					action.actionPerformed( new ActionEvent( this, 0, "select" ) );
				}
			}
		});

		final JButton up = (JButton) engine.find( "up" );
		up.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent e ){
				if ( paths.size() > 1 ){
					paths.remove( paths.size() - 1 );
					list.setFile( (File) paths.get( paths.size() - 1 ) );
					path.setText( getPath() );
				}
			}
		});

		final JButton cancel = (JButton) engine.find( "cancel" );
		cancel.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent e ){
				setOption( CANCEL );
				RelativeFileChooser.this.setVisible( false );
			}
		});

		final JButton ok = (JButton) engine.find( "ok" );
		ok.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent e ){
				paths.add( files.getSelectedValue() );
				setOption( OK );
				RelativeFileChooser.this.setVisible( false );
			}
		});
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
		StringBuffer b = new StringBuffer();
		for ( Iterator it = paths.iterator(); it.hasNext(); ){
			File file = (File) it.next();
			b.append( file.getName() );
			if ( file.isDirectory() ){
				b.append( "/" );
			}
		}
		return b.toString();
	}

}
