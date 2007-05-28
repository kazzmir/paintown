package com.rafkind.paintown;

import java.util.*;
import java.awt.*;
import java.awt.image.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import java.io.*;

import com.rafkind.paintown.level.Level;

import org.swixml.SwingEngine;

public class Editor extends JFrame {

	private Image levelImage;

	public Editor(){
		this.setSize( 900, 600 );

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

		levelImage = new BufferedImage( 1000, 300, BufferedImage.TYPE_INT_RGB );
		Graphics g = levelImage.getGraphics();
		g.setColor( new Color( 64, 192, 54 ) );
		g.fillRect( 10, 10, 200, 200 );

		quit.addActionListener( new ActionListener(){
			public void actionPerformed( ActionEvent event ){
				closeHook.invoke_();
			}
		});

		SwingEngine engine = new SwingEngine( "main.xml" );
		this.getContentPane().add( (JPanel) engine.getRootComponent() );

		final Level level = new Level();
		
		final JPanel viewContainer = (JPanel) engine.find( "view" );
		final JScrollPane viewScroll = new JScrollPane( JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS );
		final JPanel view = new MyCanvas(){
			protected void paintComponent( Graphics g ){
				JScrollBar h = viewScroll.getHorizontalScrollBar();
				JScrollBar v = viewScroll.getVerticalScrollBar();
				level.render( g, h.getValue(), 0, h.getVisibleAmount(), v.getVisibleAmount() );
			}
		};
		viewScroll.setPreferredSize( new Dimension( 200, 200 ) );
		viewScroll.setViewportView( view );
		viewScroll.getHorizontalScrollBar().setBackground( new Color( 128, 255, 0 ) );

		view.addMouseListener( new MouseInputAdapter(){
			public void mouseClicked( MouseEvent event ){
				System.out.println( "Mouse clicked at " + event.getX() + " " + event.getY() );
			}
		});

		/*
		viewScroll.getHorizontalScrollBar().addAdjustmentListener( new AdjustmentListener(){
			public void adjustmentValueChanged( AdjustmentEvent e ){
				viewScroll.repaint();
			}
		});
		viewScroll.getVerticalScrollBar().addAdjustmentListener( new AdjustmentListener(){
			public void adjustmentValueChanged( AdjustmentEvent e ){
				viewScroll.repaint();
			}
		});
		*/

		GridBagLayout layout = new GridBagLayout();
		viewContainer.setLayout( layout );
		GridBagConstraints constraints = new GridBagConstraints();
		constraints.fill = GridBagConstraints.BOTH;
		
		constraints.weightx = 1;
		constraints.weighty = 1;
		layout.setConstraints( viewScroll, constraints );
		view.setBorder( BorderFactory.createLineBorder( new Color( 255, 0, 0 ) ) );
		viewContainer.add( viewScroll );

		final JList list = (JList) engine.find( "files" );
		final DirectoryModel model = new DirectoryModel( "data" );
		list.setModel( model );

		list.addMouseListener( new MouseAdapter(){
			public void mouseClicked( MouseEvent event ){
				if ( event.getClickCount() == 2 ){
					int i = list.locationToIndex( event.getPoint() );
					if ( i != -1 ){
						File f = (File) model.getElementAt( i );
						if ( f.isDirectory() ){
							model.setDirectory( f );
						} else {
							loadLevel( f );
						}
					}
				}
			}
		});

		this.setJMenuBar( menuBar );
		this.addWindowListener( new CloseHook( closeHook ) );
	}

	private class MyCanvas extends JPanel implements Scrollable {
		public MyCanvas(){
			super();
		}

		public Dimension getPreferredSize(){
			return new Dimension( 1000, 480 );
		}

		public Dimension getPreferredScrollableViewportSize(){
			return getPreferredSize();
		}

		public int getScrollableBlockIncrement(Rectangle visibleRect, int orientation, int direction){
		 	return 10;
		}

		public boolean getScrollableTracksViewportHeight(){
			return false;
		}

		public boolean getScrollableTracksViewportWidth(){
			return false;
		}

		public int getScrollableUnitIncrement(Rectangle visibleRect, int orientation, int direction){
			return 1;
		}
	}

	private void loadLevel( File f ){
	}

	private void renderLevel( Graphics g, int x, int y, int width, int height ){
		// Image i = new BufferedImage( 640, 480, BufferedImage.TYPE_INT_RGB );
		// Graphics ig = i.getGraphics();
		g.clearRect( x, y, width, height );
		g.setColor( new Color( 255, 0, 0 ) );
		g.fillOval( 50, 50, 50, 50 );
		// g.drawImage( i, x, y, Color.WHITE, null );
		g.setColor( new Color( 0, 0, 0 ) );
		g.fillOval( 80, 52, 20, 20 );
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
