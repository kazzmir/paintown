package com.rafkind.paintown;

import javax.swing._
import java.awt._
import java.awt.image._
import java.awt.event._

class NewEditor extends JFrame {
  construct();

  def construct(){
    val screen = Toolkit.getDefaultToolkit().getScreenSize()
    this.setSize((screen.getWidth() * 4.0/5.0).toInt,
                 (screen.getHeight() * 4.0/5.0).toInt);

    Closer.open();
    val menuBar = new JMenuBar();
    val menuProgram = new JMenu("Program");
    val quit = new JMenuItem( "Quit" );
    val data = new JMenuItem( "Data path" );
    val animationEditor = new JMenuItem("Run character animation editor");
    menuProgram.add(animationEditor);
    menuProgram.add(data);
    menuProgram.add(quit);
    menuBar.add(menuProgram);
    val menuLevel = new JMenu( "Level" );
    menuBar.add(menuLevel);
    /*
        final Lambda0 closeHook = new Lambda0(){
            public Object invoke(){
                Closer.close();
                return null;
            }
        };
        */
    val newLevel = new JMenuItem("New Level");
    menuLevel.add(newLevel);
    val loadLevel = new JMenuItem("Open Level");
    menuLevel.add(loadLevel);
    val saveLevel = new JMenuItem("Save Level");
    menuLevel.add(saveLevel);
    val saveLevelAs = new JMenuItem("Save Level As");
    menuLevel.add(saveLevelAs);
    val closeLevel = new JMenuItem("Close Level");
    menuLevel.add(closeLevel);
    menuProgram.setMnemonic( KeyEvent.VK_P );
    data.setMnemonic( KeyEvent.VK_D );
    quit.setMnemonic( KeyEvent.VK_Q );
    quit.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Q, Event.CTRL_MASK));
    newLevel.setMnemonic( KeyEvent.VK_N );
    newLevel.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_N, Event.CTRL_MASK));
    menuLevel.setMnemonic( KeyEvent.VK_L );
    saveLevel.setMnemonic( KeyEvent.VK_S );
    saveLevel.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, Event.CTRL_MASK));
    saveLevelAs.setMnemonic( KeyEvent.VK_A );
    saveLevelAs.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_A, Event.CTRL_MASK));
    loadLevel.setMnemonic( KeyEvent.VK_O );
    loadLevel.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, Event.CTRL_MASK));
    closeLevel.setMnemonic( KeyEvent.VK_W );
    closeLevel.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_W, Event.CTRL_MASK));


/*
        final JTabbedPane tabbed = new JTabbedPane();
        this.getContentPane().add( tabbed );

        quit.addActionListener( new ActionListener(){
            public void actionPerformed( ActionEvent event ){
                closeHook.invoke_();
            }
        });

        animationEditor.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                String[] args = new String[0];
                com.rafkind.paintown.animator.Animator.main(args);
            }
        });

        data.addActionListener( new ActionListener(){
            public void actionPerformed( ActionEvent event ){
                / * just a container for an object * /
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
                box.set(Data.getDataPath());
                path.setText(Data.getDataPath().getPath() );
                final JButton change = (JButton) engine.find( "change" );
                change.addActionListener( new AbstractAction(){
                    public void actionPerformed( ActionEvent event ){
                        JFileChooser chooser = new JFileChooser( new File( "." ) );	
                        chooser.setFileSelectionMode( JFileChooser.DIRECTORIES_ONLY );
                        int returnVal = chooser.showOpenDialog( Editor.this );
                        if ( returnVal == JFileChooser.APPROVE_OPTION ){
                            final File newPath = chooser.getSelectedFile();
                            path.setText( newPath.getPath() );
                            box.set( newPath );
                        }
                    }
                });
                final JButton save = (JButton) engine.find( "save" );
                final JButton cancel = (JButton) engine.find( "cancel" );
                final JDialog dialog = new JDialog( Editor.this, "Paintown data path" );
                save.addActionListener( new AbstractAction(){
                    public void actionPerformed( ActionEvent event ){
                        Data.setDataPath( (File) box.get() );
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

        final HashMap levels = new HashMap();

        final Lambda2 doSave = new Lambda2(){
            public Object invoke( Object level_, Object file_ ) throws IOException {
                final Level level = (Level) level_;
                final File file = (File) file_;
                FileOutputStream out = new FileOutputStream( file );
                new PrintStream( out ).print( level.toToken().toString() + "\n" );
                out.close();
                System.out.println( level.toToken().toString() );
                return null;
            }
        };

        newLevel.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                Level level = new Level();
                / * add 3 blocks to get the user started * /
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
                        file = userSelectFile("Save level");
                    }
                    / * write the text to a file * /
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
                    File file = userSelectFile("Save level as");
                    / * write the text to a file * /
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
                        showError( "Could not load " + f.getName() );
                        System.out.println( "Could not load " + f.getName() );
                        le.printStackTrace();
                    }
                }
            }
        });
        */

    this.setJMenuBar(menuBar);
    this.addWindowListener(new WindowAdapter{
      override def windowClosing(e:WindowEvent){
        Closer.close();
      }
    });
  }
}

object Editor2 {
  def main(args: Array[String]):Unit = {
    val editor = new NewEditor();
    SwingUtilities.invokeLater(new Runnable(){
        def run(){
          editor.setVisible(true);
        }
      });
  }
}
