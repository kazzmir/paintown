package com.rafkind.paintown;

import java.util.*;
import java.awt.*;
import java.awt.image.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import java.io.*;

import com.rafkind.paintown.exception.LoadException;

import com.rafkind.paintown.level.Level;
import com.rafkind.paintown.level.Block;

import org.swixml.SwingEngine;

public class Editor extends JFrame {

	private Image levelImage;

	public Editor(){
		this.setSize( 900, 500 );

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

		/*
		levelImage = new BufferedImage( 1000, 300, BufferedImage.TYPE_INT_RGB );
		Graphics g = levelImage.getGraphics();
		g.setColor( new Color( 64, 192, 54 ) );
		g.fillRect( 10, 10, 200, 200 );

		quit.addActionListener( new ActionListener(){
			public void actionPerformed( ActionEvent event ){
				closeHook.invoke_();
			}
		});
		*/

		SwingEngine engine = new SwingEngine( "main.xml" );
		this.getContentPane().add( (JPanel) engine.getRootComponent() );

		final Level level = new Level();
		
		final JPanel viewContainer = (JPanel) engine.find( "view" );
		final JScrollPane viewScroll = new JScrollPane( JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS );
		final JPanel view = new JPanel(){

			public Dimension getPreferredSize(){
				return level.getSize();
			}

			protected void paintComponent( Graphics g ){
				JScrollBar h = viewScroll.getHorizontalScrollBar();
				JScrollBar v = viewScroll.getVerticalScrollBar();
				level.render( (Graphics2D) g, h.getValue(), 0, h.getVisibleAmount(), v.getVisibleAmount() );
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
		class BlockModel implements ListModel {
			private List objects;
			private List enabled;
			public BlockModel(){
				objects = new ArrayList();
				enabled = new ArrayList();
			}
		}
		*/

		JTabbedPane tabbed = (JTabbedPane) engine.find( "tabbed" );
		final Box blocks = Box.createVerticalBox();
		tabbed.add( "Blocks", new JScrollPane( blocks ) );
		
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

		JButton render = (JButton) engine.find( "render" );
		render.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				view.repaint();
			}
		});

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
							try{
								level.load( f );
								blocks.removeAll();
								int n = 1;
								for ( Iterator it = level.getBlocks().iterator(); it.hasNext(); ){
									Block b = (Block) it.next();
									Box stuff = Box.createHorizontalBox();
									JCheckBox check = new JCheckBox();
									check.setSelected( true );
									stuff.add( check );
									stuff.add( new JLabel( "Block " + n + " : " + b.getLength() ) );
									stuff.add( Box.createHorizontalGlue() );
									blocks.add( stuff );
									n += 1;
								}
								blocks.repaint();
								viewScroll.repaint();
							} catch ( LoadException le ){
								System.out.println( "Could not load " + f.getName() );
								le.printStackTrace();
							}
						}
					}
				}
			}
		});

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
