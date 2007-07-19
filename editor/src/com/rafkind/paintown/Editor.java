package com.rafkind.paintown;

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

public class Editor extends JFrame {

	/* global thing for copy/pasting */
	private Thing copy;

	public Editor(){
		super( "Paintown Editor" );
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
		JMenuItem newLevel = new JMenuItem( "New Level" );
		menuLevel.add( newLevel );
		JMenuItem loadLevel = new JMenuItem( "Open Level" );
		menuLevel.add( loadLevel );
		JMenuItem saveLevel = new JMenuItem( "Save Level" );
		menuLevel.add( saveLevel );
		JMenuItem saveLevelAs = new JMenuItem( "Save Level As" );
		menuLevel.add( saveLevelAs );
		JMenuItem closeLevel = new JMenuItem( "Close Level" );
		menuLevel.add( closeLevel );

		newLevel.setMnemonic( KeyEvent.VK_N );
		menuLevel.setMnemonic( KeyEvent.VK_L );
		saveLevel.setMnemonic( KeyEvent.VK_S );
		saveLevelAs.setMnemonic( KeyEvent.VK_A );
		loadLevel.setMnemonic( KeyEvent.VK_O );
		closeLevel.setMnemonic( KeyEvent.VK_C );

		final JTabbedPane tabbed = new JTabbedPane();
		this.getContentPane().add( tabbed );

		quit.addActionListener( new ActionListener(){
			public void actionPerformed( ActionEvent event ){
				closeHook.invoke_();
			}
		});

		final HashMap levels = new HashMap();

		final Lambda2 doSave = new Lambda2(){
			public Object invoke( Object level_, Object file_ ) throws IOException {
				final Level level = (Level) level_;
				final File file = (File) file_;
				FileOutputStream out = new FileOutputStream( file );
				new PrintStream( out ).print( level.toToken().toString() );
				out.close();
				System.out.println( level.toToken().toString() );
				return null;
			}
		};

		newLevel.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				Level level = new Level();
				/* add 3 blocks to get the user started */
				level.getBlocks().add( new Block() );
				level.getBlocks().add( new Block() );
				level.getBlocks().add( new Block() );
				
				levels.put( tabbed.add( createEditPanel( level ) ), level );
			}
		});

		saveLevel.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				if ( tabbed.getSelectedComponent() != null ){
					Level level = (Level) levels.get( tabbed.getSelectedComponent() );
					File file = level.getPath();
					if ( file == null ){
						file = userSelectFile();
					}
					/* write the text to a file */
					if ( file != null ){
						try{
							doSave.invoke( level, file );
							level.setPath( file );
							tabbed.setTitleAt( tabbed.getSelectedIndex(), file.getName() );
						} catch ( Exception e ){
							e.printStackTrace();
							showError( "Could not save " + file + " because " + e.getMessage() );
						}
					}
				}
			}
		});

		saveLevelAs.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				if ( tabbed.getSelectedComponent() != null ){
					Level level = (Level) levels.get( tabbed.getSelectedComponent() );
					File file = userSelectFile();
					/* write the text to a file */
					if ( file != null ){
						try{
							doSave.invoke( level, file );
							level.setPath( file );
							tabbed.setTitleAt( tabbed.getSelectedIndex(), file.getName() );
						} catch ( Exception e ){
							e.printStackTrace();
							showError( "Could not save " + file + " because " + e.getMessage() );
						}
					}
				}
			}
		});

		closeLevel.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				if ( tabbed.getSelectedComponent() != null ){
					levels.remove( tabbed.getSelectedComponent() );
					tabbed.remove( tabbed.getSelectedComponent() );
				}
			}
		});

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
						Level level = new Level( f );
						levels.put( tabbed.add( f.getName(), createEditPanel( level ) ), level );
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

	private Thing getCopy(){
		return copy;
	}

	private void setCopy( Thing t ){
		copy = t;
	}

	private File userSelectFile(){
		JFileChooser chooser = new JFileChooser( new File( "." ) );
		int returnVal = chooser.showOpenDialog( Editor.this );
		if ( returnVal == JFileChooser.APPROVE_OPTION ){
			return chooser.getSelectedFile();
		} else {
			return null;
		}
	}

	/* provide default list of objects that can be added to the level */
	private List defaultObjects(){
		List data = new ArrayList();
		data.add( new File( "data/chars/angel/angel.txt" ) );
		data.add( new File( "data/chars/billy/billy.txt" ) );
		data.add( new File( "data/chars/eiji/eiji.txt" ) );
		data.add( new File( "data/chars/heavy/heavy.txt" ) );
		data.add( new File( "data/chars/jhun/jhun.txt" ) );
		data.add( new File( "data/chars/joe/joe.txt" ) );
		data.add( new File( "data/chars/punk/punk.txt" ) );
		data.add( new File( "data/chars/ralf/ralf.txt" ) );
		data.add( new File( "data/chars/robert/robert.txt" ) );
		data.add( new File( "data/chars/rugal/rugal.txt" ) );
		data.add( new File( "data/chars/shermie/shermie.txt" ) );
		data.add( new File( "data/chars/yamazaki/yamazaki.txt" ) );
		data.add( new File( "data/chars/yashiro/yashiro.txt" ) );
		data.add( new File( "data/misc/apple/apple.txt" ) );
		return data;
	}

	private JPanel createEditPanel( final Level level ){
		final SwingEngine engine = new SwingEngine( "main.xml" );
		
		final JPanel viewContainer = (JPanel) engine.find( "view" );
		final JScrollPane viewScroll = new JScrollPane( JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS );
		final JPanel view = new JPanel(){

			public Dimension getPreferredSize(){
				return level.getSize();
			}

			protected void paintComponent( Graphics g ){
				JScrollBar h = viewScroll.getHorizontalScrollBar();
				JScrollBar v = viewScroll.getVerticalScrollBar();
				g.setColor( new Color( 64, 64, 64 ) );
				g.fillRect( 0, 0, (int) level.getWidth(), v.getVisibleAmount() );
				g.clearRect( 0, (int) v.getVisibleAmount() + 1, (int) level.getWidth(), (int) level.getHeight() );
				level.render( (Graphics2D) g, h.getValue(), 0, h.getVisibleAmount(), v.getVisibleAmount() );
			}
		};
		
		viewScroll.setPreferredSize( new Dimension( 200, 200 ) );
		viewScroll.setViewportView( view );
		viewScroll.getHorizontalScrollBar().setBackground( new Color( 128, 255, 0 ) );

		final Lambda1 editSelected = new Lambda1(){
			public Object invoke( Object t ){
				Thing thing = (Thing) t;
				final JDialog dialog = new JDialog( Editor.this, "Edit" );
				dialog.setSize( 300, 300 );
				PropertyEditor editor = thing.getEditor();
				dialog.getContentPane().add( editor.createPane( level, new Lambda0(){
					public Object invoke(){
						dialog.setVisible( false );
						viewScroll.repaint();
						return null;
					}
				}));
				dialog.setVisible( true );
				return null;
			}
		};

		class ObjectListModel implements ListModel{
			private List data;
			private List listeners;
			public ObjectListModel(){
				this.data = defaultObjects();
				this.listeners = new ArrayList();
			}

			public void add( File file ){
				data.add( file );
				ListDataEvent event = new ListDataEvent( this, ListDataEvent.INTERVAL_ADDED, data.size(), data.size() );
				for ( Iterator it = listeners.iterator(); it.hasNext(); ){
					ListDataListener l = (ListDataListener) it.next();
					l.intervalAdded( event );
				}
			}

			public void remove( int index ){
				data.remove( index );
				ListDataEvent event = new ListDataEvent( this, ListDataEvent.INTERVAL_REMOVED, index, index );
				for ( Iterator it = listeners.iterator(); it.hasNext(); ){
					ListDataListener l = (ListDataListener) it.next();
					l.intervalAdded( event );
				}

			}

			public List getAll(){
				return data;
			}

			public void addListDataListener( ListDataListener l ){
				listeners.add( l );
			}

			public Object getElementAt( int index ){
				return this.data.get( index );
			}

			public int getSize(){
				return this.data.size();
			}

			public void removeListDataListener( ListDataListener l ){
				this.listeners.remove( l );
			}
		};

		final ObjectListModel objectsModel = new ObjectListModel();

		class Mouser extends MouseMotionAdapter implements MouseInputListener {
			Thing selected = null;
			double dx, dy;
			double sx, sy;
			Popup currentPopup;

			public Thing getSelected(){
				return selected;
			}

			public void setSelected( Thing t ){
				selected = t;
			}

			public void mouseDragged( MouseEvent event ){
				
				if ( selected != null ){
					// System.out.println( "sx,sy: " + sx + ", " + sy + " ex,ey: " + (event.getX() / 2) + ", " + (event.getY() / 2) + " dx, dy: " + dx + ", " + dy );
					level.moveThing( selected, (int)(sx + event.getX() / level.getScale() - dx), (int)(sy + event.getY() / level.getScale() - dy) );
					viewScroll.repaint();
				}
			}

			private boolean leftClick( MouseEvent event ){
				return event.getButton() == MouseEvent.BUTTON1;
			}
			
			private boolean rightClick( MouseEvent event ){
				return event.getButton() == MouseEvent.BUTTON3;
			}

			private void selectThing( MouseEvent event ){
				Thing t = findThingAt( event );
				Block has = null;
				for ( Iterator it = level.getBlocks().iterator(); it.hasNext(); ){
					Block b = (Block) it.next();
					b.setHighlight( false );
					if ( t != null && b.hasThing( t ) ){
						has = b;
					 }
				}

				if ( has != null ){
					has.setHighlight( true );
					viewScroll.repaint();
				}

				if ( selected == null && t != null ){
					// selected = findThingAt( event );
					selected = t;
					selected.setSelected( true );
					sx = selected.getX();
					sy = selected.getY() + level.getMinZ();
					// System.out.println( "Y: " + selected.getY() + " minZ: " + level.getMinZ() );
					dx = event.getX() / level.getScale();
					dy = event.getY() / level.getScale();
					// System.out.println( "Found: " + selected + " at " + event.getX() + " " + event.getY() );
				}

				if ( getSelected() != null && event.getClickCount() == 2 ){
					editSelected.invoke_( getSelected() );
					// System.out.println( "Properties of " + getSelected() );
				}
			}

			private List findFiles( File dir, final String ending ){
				File[] all = dir.listFiles( new java.io.FileFilter(){
					public boolean accept( File path ){
						return path.isDirectory() || path.getName().endsWith( ending );
					}
				});
				List files = new ArrayList();
				for ( int i = 0; i < all.length; i++ ){
					if ( all[ i ].isDirectory() ){
						files.addAll( findFiles( all[ i ], ending ) );
					} else {
						files.add( all[ i ] );
					}
				}
				return files;
			}

			private Block findBlock( MouseEvent event ){
				int x = (int)(event.getX() / level.getScale());
				// System.out.println( event.getX() + " -> " + x );
				int total = 0;
				for ( Iterator it = level.getBlocks().iterator(); it.hasNext(); ){
					Block b = (Block) it.next();
					if ( b.isEnabled() ){
						if ( x >= total && x <= total + b.getLength() ){
							return b;
						}
						total += b.getLength();
					}
				}
				return null;
			}

			private Thing makeThing( Token head, int x, int y, String path ) throws LoadException {
				if ( head.getName().equals( "character" ) ){
					Token temp = new Token();
					temp.addToken( new Token( "character" ) );
					temp.addToken( new String[]{ "name", "TempName" } );
					temp.addToken( new String[]{ "coords", String.valueOf( x ), String.valueOf( y ) } );
					temp.addToken( new String[]{ "health", "40" } );
					temp.addToken( new String[]{ "path", path } );
					return new Character( temp );
				} else if ( head.getName().equals( "item" ) ){
					Token temp = new Token();
					temp.addToken( new Token( "item" ) );
					temp.addToken( new String[]{ "coords", String.valueOf( x ), String.valueOf( y ) } );
					temp.addToken( new String[]{ "path", path } );
					// System.out.println( "Make item from " + temp.toString() );
					return new Item( temp );
				}
				throw new LoadException( "Unknown type: " + head.getName() );
			}

			private Vector collectCharFiles(){
				return new Vector( objectsModel.getAll() );
			}

			private void showAddObjectPopup( final MouseEvent event ){
				// JPanel panel = new JPanel();
				final Vector files = collectCharFiles();
				Box panel = Box.createVerticalBox();
				final JList all = new JList( files );
				panel.add( new JScrollPane( all ) );
				JButton add = new JButton( "Add" );
				JButton close = new JButton( "Close" );
				Box buttons = Box.createHorizontalBox();
				buttons.add( add );
				buttons.add( close );
				panel.add( buttons );
				if ( currentPopup != null ){
					currentPopup.hide();
				}
				// Point px = viewContainer.getLocationOnScreen();
				final Popup p = PopupFactory.getSharedInstance().getPopup( Editor.this, panel, event.getX() - viewScroll.getHorizontalScrollBar().getValue(), event.getY() );
				// final Popup p = PopupFactory.getSharedInstance().getPopup( Editor.this, panel, 100, 100 );
				close.addActionListener( new AbstractAction(){
					public void actionPerformed( ActionEvent event ){
						p.hide();
					}
				});
				currentPopup = p;
				p.show();

				final Lambda1 addThing = new Lambda1(){
					public Object invoke( Object f ){
						File file = (File) f;
						try{
							Block b = findBlock( event );
							if ( b != null ){
								TokenReader reader = new TokenReader( file );
								Token head = reader.nextToken();
								int x = (int)(event.getX() / level.getScale());
								int y = (int)(event.getY() / level.getScale());
								for ( Iterator it = level.getBlocks().iterator(); it.hasNext(); ){
									Block b1 = (Block) it.next();
									if ( b1 == b ){
										break;
									}
									if ( b1.isEnabled() ){
										x -= b1.getLength();
									}
								}
								b.addThing( makeThing( head, x, y, file.getPath() ) );
								/*
									Character c = new Character( reader.nextToken() );
									b.add( new Character( reader.nextToken() ) );
									*/
								viewScroll.repaint();
							} else {
								// JOptionPane.showMessageDialog( null, "The cursor is not within a block. Either move the cursor or add a block.", "Paintown Editor Error", JOptionPane.ERROR_MESSAGE );
								showError( "The cursor is not within a block. Either move the cursor or add a block." );
							}
						} catch ( LoadException e ){
							System.out.println( "Could not load " + file );
							e.printStackTrace();
						}

						return null;
					}
				};

				all.addMouseListener( new MouseAdapter() {
					public void mouseClicked( MouseEvent clicked ){
						if ( clicked.getClickCount() == 2 ){
							int index = all.locationToIndex( clicked.getPoint() );
							File f = (File) files.get( index );
							addThing.invoke_( f );
							p.hide();
						}
					}
				});

				add.addActionListener( new AbstractAction(){
					public void actionPerformed( ActionEvent event ){
						int index = all.getSelectedIndex();
						if ( index != -1 ){
							File f = (File) files.get( index );
							addThing.invoke_( f );
							p.hide();
						}
					}
				});
			}
			
			public void mousePressed( MouseEvent event ){
				if ( leftClick( event ) ){
					if ( selected != null ){
						selected.setSelected( false );
					}
					selected = null;
					selectThing( event );
				} else if ( rightClick( event ) ){
					showAddObjectPopup( event );
				}
			}
			
			public void mouseExited( MouseEvent event ){
				if ( selected != null ){
					// selected = null;
					viewScroll.repaint();
				}
			}

			private Thing findThingAt( MouseEvent event ){
				return level.findThing( (int)(event.getX() / level.getScale()), (int)(event.getY() / level.getScale()) );
			}

			public void mouseClicked( MouseEvent event ){
			}
			
			public void mouseEntered( MouseEvent event ){
			}
			
			public void mouseReleased( MouseEvent event ){
				if ( selected != null ){
					// selected = null;
					viewScroll.repaint();
				}
			}
		}

		final Mouser mousey = new Mouser();

		view.addMouseMotionListener( mousey );
		view.addMouseListener( mousey );
		view.addMouseListener( new MouseAdapter(){
			public void mouseClicked( MouseEvent event ){
				/* force focus to move to the view */
				view.requestFocusInWindow(); 
			}
		});

		JTabbedPane tabbed = (JTabbedPane) engine.find( "tabbed" );
		final Box holder = Box.createVerticalBox();
		final Box blocks = Box.createVerticalBox();
		holder.add( new JScrollPane( blocks ) );

		holder.add( new JSeparator() );

		class ObjectList implements ListModel {
			/* list listeners */
			private List listeners;
			private List things;
			private Lambda1 update;
			public ObjectList(){
				listeners = new ArrayList();
				things = new ArrayList();
				update = new Lambda1(){
					public Object invoke( Object t ){
						Thing thing = (Thing) t;
						int count = 0;
						for ( Iterator it = things.iterator(); it.hasNext(); count += 1 ){
							Thing current = (Thing) it.next();
							if ( current == thing ){
								contentsChanged( new ListDataEvent( this, ListDataEvent.CONTENTS_CHANGED, count, count ) );
							}
						}
						return null;
					}
				};
			}

			public void setBlock( Block b ){
				if ( b == null ){
					this.things = new ArrayList();
				} else {
					this.things = b.getThings();
					for ( Iterator it = things.iterator(); it.hasNext(); ){
						Thing t = (Thing) it.next();
						t.addListener( this.update );
					}
				}

				contentsChanged( new ListDataEvent( this, ListDataEvent.CONTENTS_CHANGED, 0, 999999 ) );
			}

			private void contentsChanged( ListDataEvent event ){
				for ( Iterator it = listeners.iterator(); it.hasNext(); ){
					ListDataListener l = (ListDataListener) it.next();
					l.contentsChanged( event );
				}
			}

			/*
			public void update( int index ){
				ListDataEvent event = new ListDataEvent( this, ListDataEvent.CONTENTS_CHANGED, index, index + 1 );
				for ( Iterator it = listeners.iterator(); it.hasNext(); ){
						  ListDataListener l = (ListDataListener) it.next();
						  l.contentsChanged( event );
				}
			}
			*/

			public void addListDataListener( ListDataListener l ){
				listeners.add( l );
			}

			public Object getElementAt( int index ){
				return this.things.get( index );
			}

			public int getSize(){
				return this.things.size();
			}

			public void removeListDataListener( ListDataListener l ){
				this.listeners.remove( l );
			}
		}

		final ObjectList objectList = new ObjectList();
		final JList currentObjects = new JList( objectList );
		holder.add( new JLabel( "Objects" ) );
		holder.add( new JScrollPane( currentObjects ) );
		holder.add( Box.createVerticalGlue() );

		currentObjects.addListSelectionListener( new ListSelectionListener(){
			public void valueChanged( ListSelectionEvent e ){
				Thing t = (Thing) currentObjects.getSelectedValue();
				if ( mousey.getSelected() != null ){
					Thing old = mousey.getSelected();
					old.setSelected( false );
					level.findBlock( old ).setHighlight( false );
				}
				t.setSelected( true );
				mousey.setSelected( t );

				/* the current X position within the world */
				int currentX = 0;
				Block b = level.findBlock( t );
				b.setHighlight( true );

				/* calculate absolute X position of the selected thing */
				for ( Iterator it = level.getBlocks().iterator(); it.hasNext(); ){
					Block next = (Block) it.next();
					if ( next == b ){
						break;
					}
					if ( next.isEnabled() ){
						currentX += next.getLength();
					}
				}

				/* adjust X position a little bit to show the entire thing */
				currentX += t.getX() - t.getWidth();

				/* scroll over to the selected thing */
				viewScroll.getHorizontalScrollBar().setValue( (int)(currentX * level.getScale()) );

				viewScroll.repaint();
			}
		});

		currentObjects.getInputMap().put( KeyStroke.getKeyStroke( KeyEvent.VK_DELETE, 0 ), "delete" );
		currentObjects.getActionMap().put( "delete", new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				Thing t = (Thing) currentObjects.getSelectedValue();
				if ( t != null ){
					mousey.setSelected( null );
					Block b = level.findBlock( t );
					b.removeThing( t );
					objectList.setBlock( b );
					viewScroll.repaint();
				}
			}
		});

		currentObjects.addMouseListener( new MouseAdapter() {
			public void mouseClicked( MouseEvent clicked ){
				if ( clicked.getClickCount() == 2 ){
					Thing t = (Thing) currentObjects.getSelectedValue();	
					editSelected.invoke_( t );
				}
			}
		});

		/* so the user can click on the scrolly pane */
		// viewScroll.setFocusable( true );
		view.setFocusable( true );

		view.getInputMap().put( KeyStroke.getKeyStroke( KeyEvent.VK_DELETE, 0 ), "delete" );

		/* ctrl-c */
		view.getInputMap().put( KeyStroke.getKeyStroke( KeyEvent.VK_C, 2 ), "copy" );
		/* ctrl-v */
		view.getInputMap().put( KeyStroke.getKeyStroke( KeyEvent.VK_V, 2 ), "paste" );

		view.getActionMap().put( "delete", new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				if ( mousey.getSelected() != null ){
					Block b = level.findBlock( mousey.getSelected() );
					b.removeThing( mousey.getSelected() );
					mousey.setSelected( null );
					objectList.setBlock( b );
					viewScroll.repaint();
				}
			}
		});
		
		view.getActionMap().put( "copy", new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				// System.out.println( "Copy object" );
				if ( mousey.getSelected() != null ){
					setCopy( mousey.getSelected().copy() );
				}
			}
		});

		view.getActionMap().put( "paste", new AbstractAction(){
			private int calculateLength( List blocks ){
				int total = 0;
				for ( Iterator it = blocks.iterator(); it.hasNext(); ){
					Block b = (Block) it.next();
					total += b.getLength();
				}
				return total;
			}

			public void actionPerformed( ActionEvent event ){
				/* middle of the current screen */
				int x = (int)(viewScroll.getHorizontalScrollBar().getValue() / level.getScale() + viewScroll.getHorizontalScrollBar().getVisibleAmount() / level.getScale() / 2 );
				/* in between the min and max z lines */
				int y = (int)((level.getMaxZ() - level.getMinZ()) / 2);
				Block b = level.findBlock( x );
				if ( b != null && getCopy() != null ){
					Thing copy = getCopy().copy();
					/* x has to be relative to the beginning of the block */
					copy.setX( x - calculateLength( level.getBlocks().subList( 0, level.getBlocks().indexOf( b ) ) ) );
					copy.setY( y );
					b.addThing( copy );
					objectList.setBlock( b );
					viewScroll.repaint();
				} else {
					System.out.println( "No block found at " + x );
				}
			}
		});

		tabbed.add( "Blocks", holder );
		
		final SwingEngine objectEngine = new SwingEngine( "objects.xml" );
		tabbed.add( "Objects", (JComponent) objectEngine.getRootComponent() );

		// final JList objects = new JList( allowableObjects );
		final JList objects = (JList) objectEngine.find( "objects" );
		/* objectsModel is declared way up top */
		objects.setModel( objectsModel  );

		{
			final JButton add = (JButton) objectEngine.find( "add" );
			final JButton remove = (JButton) objectEngine.find( "delete" );

			add.addActionListener( new AbstractAction(){
				public void actionPerformed( ActionEvent event ){
					RelativeFileChooser chooser = new RelativeFileChooser( Editor.this, new File( "." ) );
					int ret = chooser.open();
					if ( ret == RelativeFileChooser.OK ){
						final String path = chooser.getPath();
						objectsModel.add( new File( path ) );
					}
				}
			});

			remove.addActionListener( new AbstractAction(){
				public void actionPerformed( ActionEvent event ){
					int index = objects.getSelectedIndex();
					if ( index != -1 ){
						objectsModel.remove( index );
					}
				}
			});
		}

		final SwingEngine levelEngine = new SwingEngine( "level.xml" );
		final JPanel levelPane = (JPanel) levelEngine.getRootComponent();
		tabbed.add( "Level", levelPane );

		final JSpinner levelMinZ = (JSpinner) levelEngine.find( "min-z" );
		final JSpinner levelMaxZ = (JSpinner) levelEngine.find( "max-z" );
		final JTextField levelBackground = (JTextField) levelEngine.find( "background" );
		final JButton levelChangeBackground = (JButton) levelEngine.find( "change-background" );
		final Vector frontPanelsData = new Vector();
		final JList frontPanels = (JList) levelEngine.find( "front-panels" );
		frontPanels.setListData( frontPanelsData );
		final Vector backPanelsData = new Vector();
		final JList backPanels = (JList) levelEngine.find( "back-panels" );
		final JTextArea order = (JTextArea) levelEngine.find( "order" );
		final JComboBox pickOrder = (JComboBox) levelEngine.find( "pick-order" );

		class BackPanelCombo implements ComboBoxModel {
			private Object selected;
			private Vector data;
			private List listeners;

			public BackPanelCombo( Vector data ){
				this.data = data;
				this.listeners = new ArrayList();
			}

			public Object getSelectedItem(){
				return selected;
			}

			public void update(){
				selected = null;
				ListDataEvent event = new ListDataEvent( this, ListDataEvent.CONTENTS_CHANGED, 0, 99999 );
				for ( Iterator it = listeners.iterator(); it.hasNext(); ){
					ListDataListener l = (ListDataListener) it.next();
					l.contentsChanged( event );
				}
			}

			public void setSelectedItem( Object i ){
				selected = i;
			}

			public void addListDataListener( ListDataListener l ){
				listeners.add( l );
			}

			public Object getElementAt( int index ){
				return this.data.get( index );
			}

			public int getSize(){
				return this.data.size();
			}

			public void removeListDataListener( ListDataListener l ){
				this.listeners.remove( l );
			}
		};

		final BackPanelCombo comboModel = new BackPanelCombo( backPanelsData );
		pickOrder.setModel( comboModel );

		final Lambda0 setOrderText = new Lambda0(){
			public Object invoke(){
				StringBuffer orderText = new StringBuffer();
				for ( Iterator it = level.getBackPanelOrder().iterator(); it.hasNext(); ){
					Integer num = (Integer) it.next();
					String name = level.getBackPanelName( num.intValue() );
					orderText.append( name ).append( "\n" );
				}
				order.setText( orderText.toString() );
				return null;
			}
		};

		{
			final JButton add = (JButton) levelEngine.find( "add-order" );
			final JButton remove = (JButton) levelEngine.find( "remove-order" );
			add.addActionListener( new AbstractAction(){
				public void actionPerformed( ActionEvent event ){
					String path = (String) pickOrder.getSelectedItem();
					if ( path != null ){
						level.addBackPanelOrder( path );
						setOrderText.invoke_();
						viewScroll.repaint();
					}
				}
			});

			remove.addActionListener( new AbstractAction(){
				public void actionPerformed( ActionEvent event ){
					level.removeLastOrder();
					setOrderText.invoke_();
					viewScroll.repaint();
				}
			});
		}

		{ /* force scope */
			final JButton add = (JButton) levelEngine.find( "add-front-panel" );
			add.addActionListener( new AbstractAction(){
				public void actionPerformed( ActionEvent event ){
					RelativeFileChooser chooser = new RelativeFileChooser( Editor.this, new File( "." ) );
					int ret = chooser.open();
					if ( ret == RelativeFileChooser.OK ){
						try{
							final String path = chooser.getPath();
							level.addFrontPanel( path );
							frontPanelsData.add( path );
							frontPanels.setListData( frontPanelsData );
							viewScroll.repaint();
						} catch ( LoadException le ){
							le.printStackTrace();
						}
					}
				}
			});

			final JButton remove = (JButton) levelEngine.find( "delete-front-panel" );
			remove.addActionListener( new AbstractAction(){
				public void actionPerformed( ActionEvent event ){
					if ( frontPanels.getSelectedValue() != null ){
						String path = (String) frontPanels.getSelectedValue();
						level.removeFrontPanel( path );
						frontPanelsData.remove( path );
						frontPanels.setListData( frontPanelsData );
						viewScroll.repaint();
					}
				}
			});
		}

		{ /* force scope */
			final JButton add = (JButton) levelEngine.find( "add-back-panel" );
			add.addActionListener( new AbstractAction(){
				public void actionPerformed( ActionEvent event ){
					RelativeFileChooser chooser = new RelativeFileChooser( Editor.this, new File( "." ) );
					int ret = chooser.open();
					if ( ret == RelativeFileChooser.OK ){
						try{
							final String path = chooser.getPath();
							level.addBackPanel( path );
							backPanelsData.add( path );
							backPanels.setListData( backPanelsData );
							comboModel.update();
							viewScroll.repaint();
						} catch ( LoadException le ){
							le.printStackTrace();
						}
					}
				}
			});

			final JButton remove = (JButton) levelEngine.find( "delete-back-panel" );
			remove.addActionListener( new AbstractAction(){
				public void actionPerformed( ActionEvent event ){
					if ( backPanels.getSelectedValue() != null ){
						String path = (String) backPanels.getSelectedValue();
						level.removeBackPanel( path );
						backPanelsData.remove( path );
						backPanels.setListData( backPanelsData );
						setOrderText.invoke_();
						comboModel.update();
						viewScroll.repaint();
					}
				}
			});
		}

		levelMinZ.setModel( new SpinnerNumberModel() );
		levelMinZ.addChangeListener( new ChangeListener(){
			public void stateChanged( ChangeEvent e ){
				JSpinner spinner = (JSpinner) e.getSource();
				Integer i = (Integer) spinner.getValue();
				level.setMinZ( i.intValue() );
				viewScroll.repaint();
			}
		});

		levelMaxZ.setModel( new SpinnerNumberModel() );
		levelMaxZ.addChangeListener( new ChangeListener(){
			public void stateChanged( ChangeEvent e ){
				JSpinner spinner = (JSpinner) e.getSource();
				Integer i = (Integer) spinner.getValue();
				level.setMaxZ( i.intValue() );
				viewScroll.repaint();
			}
		});

		levelBackground.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				level.loadBackground( levelBackground.getText() );
				viewScroll.repaint();
			}
		});

		levelChangeBackground.addActionListener( new AbstractAction(){
			public void actionPerformed( ActionEvent event ){
				RelativeFileChooser chooser = new RelativeFileChooser( Editor.this, new File( "." ) );
				int ret = chooser.open();
				if ( ret == RelativeFileChooser.OK ){
					final String path = chooser.getPath();
					level.loadBackground( path );
					levelBackground.setText( path );
					viewScroll.repaint();
				}
			}
		});

		/* initialize all the other crap for a level */
		final Lambda1 loadLevelProperties = new Lambda1(){
			public Object invoke( Object level_ ){
				Level level = (Level) level_;
				levelMinZ.setValue( new Integer( level.getMinZ() ) );
				levelMaxZ.setValue( new Integer( level.getMaxZ() ) );
				levelBackground.setText( level.getBackgroundFile() );
				frontPanelsData.clear();
				frontPanelsData.addAll( level.getFrontPanelNames() );
				frontPanels.setListData( frontPanelsData );
				backPanelsData.clear();
				backPanelsData.addAll( level.getBackPanelNames() );
				backPanels.setListData( backPanelsData );

				setOrderText.invoke_();
				comboModel.update();
				return null;
			}
		};

		/* mess around with layout nonsense */
		GridBagLayout layout = new GridBagLayout();
		viewContainer.setLayout( layout );
		GridBagConstraints constraints = new GridBagConstraints();
		constraints.fill = GridBagConstraints.BOTH;
		
		constraints.weightx = 1;
		constraints.weighty = 1;
		layout.setConstraints( viewScroll, constraints );
		view.setBorder( BorderFactory.createLineBorder( new Color( 255, 0, 0 ) ) );
		viewContainer.add( viewScroll );
		
		final Lambda2 setupBlocks = new Lambda2(){
			private void editBlockProperties( final Block block, final Lambda0 done ){
				final JDialog dialog = new JDialog( Editor.this, "Edit" );
				dialog.setSize( 200, 200 );
				final SwingEngine engine = new SwingEngine( "block.xml" );
				dialog.getContentPane().add( (JPanel) engine.getRootComponent() );

				final JTextField length = (JTextField) engine.find( "length" );
				final JTextField finish = (JTextField) engine.find( "finish" );
				final JButton save = (JButton) engine.find( "save" );
				final JButton close = (JButton) engine.find( "close" );

				length.setText( String.valueOf( block.getLength() ) );
				finish.setText( String.valueOf( block.getFinish() ) );

				save.addActionListener( new AbstractAction(){
					public void actionPerformed( ActionEvent event ){
						block.setLength( Integer.parseInt( length.getText() ) );
						block.setFinish( Integer.parseInt( finish.getText() ) );
						done.invoke_();
						dialog.setVisible( false );
					}
				});

				close.addActionListener( new AbstractAction(){
					public void actionPerformed( ActionEvent event ){
						dialog.setVisible( false );
					}
				});

				dialog.setVisible( true );
			}

			/* self_ should be the 'setupBlocks' lambda so that it can
			 * call itself recursively
			 */
			public Object invoke( Object l, Object self_ ){
				final Lambda2 self = (Lambda2) self_;
				final Level level = (Level) l;
				blocks.removeAll();
				int n = 1;
				int total = 0;
				for ( Iterator it = level.getBlocks().iterator(); it.hasNext(); ){
					final Block block = (Block) it.next();
					Box stuff = Box.createHorizontalBox();
					JCheckBox check = new JCheckBox( new AbstractAction(){
						public void actionPerformed( ActionEvent event ){
							JCheckBox c = (JCheckBox) event.getSource();
							block.setEnabled( c.isSelected() );
							view.revalidate();
							viewScroll.repaint();
						}
					});

					check.setSelected( true );
					stuff.add( check );
					final JButton button = new JButton( "Block " + n + " : " + block.getLength() );
					button.addActionListener( new AbstractAction(){
						public void actionPerformed( ActionEvent event ){
							objectList.setBlock( block );
						}
					});
					stuff.add( button );
					stuff.add( Box.createHorizontalStrut( 3 ) );

					JButton edit = new JButton( "Edit" );
					final int xnum = n;
					edit.addActionListener( new AbstractAction(){
						public void actionPerformed( ActionEvent event ){
							editBlockProperties( block, new Lambda0(){
								public Object invoke(){
									button.setText( "Block " + xnum + " : " + block.getLength() );
									view.revalidate();
									viewScroll.repaint();
									return null;
								}
							});
						}
					});
					stuff.add( edit );
					stuff.add( Box.createHorizontalStrut( 3 ) );

					JButton erase = new JButton( "Delete" );
					erase.addActionListener( new AbstractAction(){
						public void actionPerformed( ActionEvent event ){
							mousey.setSelected( null );
							objectList.setBlock( null );
							level.getBlocks().remove( block );
							self.invoke_( level, self );
							view.repaint();
						}
					});
					stuff.add( erase );

					stuff.add( Box.createHorizontalGlue() );
					blocks.add( stuff );
					
					total += block.getLength();
					n += 1;
				}
				blocks.add( Box.createVerticalGlue() );
				Box addf = Box.createHorizontalBox();
				JButton add = new JButton( "Add block" );
				add.addActionListener( new AbstractAction(){
					public void actionPerformed( ActionEvent event ){
						Block b = new Block();
						level.getBlocks().add( b );
						self.invoke_( level, self );
						view.revalidate();
						viewScroll.repaint();
					}
				});
				addf.add( add );
				addf.add( Box.createHorizontalGlue() );
				blocks.add( addf );
				Box f = Box.createHorizontalBox();
				f.add( new JLabel( "Total length " + total ) );
				f.add( Box.createHorizontalGlue() );
				blocks.add( f );
				blocks.revalidate();
				blocks.repaint();
				return null;
			}
		};
		
		setupBlocks.invoke_( level, setupBlocks );
		loadLevelProperties.invoke_( level );

		JPanel scroll = (JPanel) engine.find( "scroll" );
		final JScrollBar scrolly = new JScrollBar( JScrollBar.HORIZONTAL, 20, 0, 1, 20 );
		final JLabel scale = (JLabel) engine.find( "scale" );
		scroll.add( scrolly );
		scrolly.addAdjustmentListener( new AdjustmentListener(){
			public void adjustmentValueChanged( AdjustmentEvent e ){
				level.setScale( (double) e.getValue() * 2.0 / scrolly.getMaximum() );
				scale.setText( "Scale: " + level.getScale() );
				view.revalidate();
				viewScroll.repaint();
			}
		});

		return (JPanel) engine.getRootComponent();
	}

	private static void showError( String message ){
		JOptionPane.showMessageDialog( null, message, "Paintown Editor Error", JOptionPane.ERROR_MESSAGE );
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
