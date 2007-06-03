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
import com.rafkind.paintown.level.Thing;
import javax.swing.filechooser.FileFilter;

import org.swixml.SwingEngine;

public class Editor extends JFrame {

	private Image levelImage;

	public Editor(){
		this.setSize( 900, 500 );

		JMenuBar menuBar = new JMenuBar();
		JMenu menuProgram = new JMenu( "Program" );
		menuBar.add( menuProgram );
		JMenu menuLevel = new JMenu( "Level" );
		menuBar.add( menuLevel );
		JMenuItem quit = new JMenuItem( "Quit" );
		menuProgram.add( quit );
		final Lambda0 closeHook = new Lambda0(){
			public Object invoke(){
				System.exit( 0 );
				return null;
			}
		};
		JMenuItem loadLevel = new JMenuItem( "Open Level" );
		menuLevel.add( loadLevel );

		/*
		levelImage = new BufferedImage( 1000, 300, BufferedImage.TYPE_INT_RGB );
		Graphics g = levelImage.getGraphics();
		g.setColor( new Color( 64, 192, 54 ) );
		g.fillRect( 10, 10, 200, 200 );
		*/

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

		class Mouser extends MouseMotionAdapter implements MouseInputListener {
			Thing selected = null;

			public void mouseDragged( MouseEvent event ){
				if ( selected == null ){
					selected = level.findThing( event.getX() / 2, event.getY() / 2 );
					System.out.println( "Found: " + selected + " at " + event.getX() + " " + event.getY() );
				}
				if ( selected != null ){
					level.moveThing( selected, event.getX() / 2, event.getY() / 2 );
				}
			}
			
			public void mousePressed( MouseEvent event ){
			}
			
			public void mouseExited( MouseEvent event ){
				if ( selected != null ){
					selected = null;
					view.repaint();
				}
			}

			public void mouseClicked( MouseEvent event ){
			}
			
			public void mouseEntered( MouseEvent event ){
			}
			
			public void mouseReleased( MouseEvent event ){
				if ( selected != null ){
					selected = null;
					view.repaint();
				}
			}
		}

		Mouser m = new Mouser();

		view.addMouseMotionListener( m );
		view.addMouseListener( m );

		/*
		view.addMouseMotionListener( new MouseMotionAdapter(){
			Thing selected = null;
			public void mouseDragged( MouseEvent event ){
				if ( selected == null ){
					selected = level.findThing( event.getX(), event.getY() );
				}
				if ( selected != null ){
					level.moveThing( selected, event.getX(), event.getY() );
				}
				// view.repaint();
				// System.out.println( "Mouse dragged at " + event.getX() + " " + event.getY() );
			}
		});

		view.addMouseListener( new MouseInputAdapter(){
			public void mousePressed( MouseEvent event ){
				// System.out.println( "Mouse pushed at " + event.getX() + " " + event.getY() );
			}

			public void mouseClicked( MouseEvent event ){
				// System.out.println( "Mouse clicked at " + event.getX() + " " + event.getY() );
			}

			public void mouseReleased( MouseEvent event ){
			}
		});
		*/

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

		/*
		final JList list = (JList) engine.find( "files" );
		final DirectoryModel model = new DirectoryModel( "data" );
		list.setModel( model );
		*/

		loadLevel.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				JFileChooser chooser = new JFileChooser( new File( "." ) );	
				chooser.setFileFilter( new FileFilter(){
					public boolean accept( File f ){
						return f.isDirectory() || f.getName().endsWith( ".txt" );
					}

					public String getDescription(){
						return "Level files";
					}
				});

				// chooser.setFileSelectionMode( JFileChooser.FILES_ONLY );
				int returnVal = chooser.showOpenDialog( Editor.this );
				if ( returnVal == JFileChooser.APPROVE_OPTION ){
					final File f = chooser.getSelectedFile();
					try{
						level.load( f );
						blocks.removeAll();
						int n = 1;
						for ( Iterator it = level.getBlocks().iterator(); it.hasNext(); ){
							final Block b = (Block) it.next();
							Box stuff = Box.createHorizontalBox();
							JCheckBox check = new JCheckBox( new AbstractAction(){
								public void actionPerformed( ActionEvent event ){
									JCheckBox c = (JCheckBox) event.getSource();
									b.setEnabled( c.isSelected() );
									view.revalidate();
									// view.repaint();
									viewScroll.repaint();
								}
							});

							check.setSelected( true );
							stuff.add( check );
							stuff.add( new JLabel( "Block " + n + " : " + b.getLength() ) );
							stuff.add( Box.createHorizontalGlue() );
							blocks.add( stuff );
							n += 1;
						}

						blocks.repaint();
						view.revalidate();
						viewScroll.repaint();
					} catch ( LoadException le ){
						System.out.println( "Could not load " + f.getName() );
						le.printStackTrace();
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
