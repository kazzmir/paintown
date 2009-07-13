package com.rafkind.paintown.animator;

import java.util.*;
import java.awt.*;
import java.awt.image.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import java.io.*;

import java.util.List;

import com.rafkind.paintown.exception.LoadException;

import javax.swing.filechooser.FileFilter;

import org.swixml.SwingEngine;

import java.awt.event.*;
import javax.swing.*;

import com.rafkind.paintown.*;

import com.rafkind.paintown.animator.IQueue;
import com.rafkind.paintown.animator.Player;
import com.rafkind.paintown.animator.SpecialPanel;
import com.rafkind.paintown.animator.DrawState;

public class Animator extends JFrame {

    private static File dataPath = new File( "data" );
    private static int CURRENT_TAB = 0;
    private static Animator animator;

    private static JTabbedPane pane = new JTabbedPane();

    public Animator() throws Exception {
        super("Paintown Animator");
        this.setSize( (int)Toolkit.getDefaultToolkit().getScreenSize().getWidth() - 100, (int)Toolkit.getDefaultToolkit().getScreenSize().getHeight() - 100 );

        animator = this;

        JMenuBar menuBar = new JMenuBar();

        JMenu menuProgram = new JMenu( "Program" );
        JMenuItem levelEditor = new JMenuItem("Run the level editor");
        JMenuItem quit = new JMenuItem( "Quit" );
        JMenuItem clearCache = new JMenuItem( "Clear image cache" );
        JMenuItem data = new JMenuItem( "Data path" );
        JMenuItem closeTab = new JMenuItem( "Close Tab" );
        menuProgram.add(levelEditor);
        menuProgram.add( data );
        menuProgram.add( clearCache );
        menuProgram.add( clearCache );
        menuProgram.add( clearCache );
        menuProgram.add( closeTab );
        menuProgram.add( quit );
        menuBar.add( menuProgram );

        JMenu menuProjectile = new JMenu( "Projectile" );
        menuBar.add( menuProjectile );

        JMenuItem newProjectile = new JMenuItem( "New Projectile" );
        menuProjectile.add( newProjectile );
        JMenuItem openProjectile = new JMenuItem( "Open Projectile" );
        menuProjectile.add( openProjectile );
        JMenuItem saveProjectile = new JMenuItem( "Save Projectile" );
        menuProjectile.add( saveProjectile );

        JMenu menuCharacter = new JMenu( "Character" );
        menuBar.add( menuCharacter );

        JMenuItem newCharacter = new JMenuItem( "New Character" );
        menuCharacter.add( newCharacter );
        JMenuItem loadCharacter = new JMenuItem( "Open Character" );
        menuCharacter.add( loadCharacter );
        JMenuItem saveCharacter = new JMenuItem( "Save Character" );
        menuCharacter.add( saveCharacter );
        JMenuItem saveCharacterAs = new JMenuItem( "Save Character As" );
        menuCharacter.add( saveCharacterAs );

        final Lambda0 closeHook = new Lambda0(){
            public Object invoke(){
                System.exit( 0 );
                return null;
            }
        };

        menuProgram.setMnemonic( KeyEvent.VK_P );
        data.setMnemonic( KeyEvent.VK_D );
        closeTab.setMnemonic( KeyEvent.VK_C );
        closeTab.setAccelerator(KeyStroke.getKeyStroke(
                    KeyEvent.VK_W, Event.CTRL_MASK));
        quit.setMnemonic( KeyEvent.VK_Q );
        quit.setAccelerator(KeyStroke.getKeyStroke(
                    KeyEvent.VK_Q, Event.CTRL_MASK));
        newCharacter.setMnemonic( KeyEvent.VK_N );
        newCharacter.setAccelerator(KeyStroke.getKeyStroke(
                    KeyEvent.VK_N, Event.CTRL_MASK));
        menuCharacter.setMnemonic( KeyEvent.VK_H );
        saveCharacter.setMnemonic( KeyEvent.VK_S );
        saveCharacter.setAccelerator(KeyStroke.getKeyStroke(
                    KeyEvent.VK_S, Event.CTRL_MASK));
        saveCharacterAs.setMnemonic( KeyEvent.VK_A );
        saveCharacterAs.setAccelerator(KeyStroke.getKeyStroke(
                    KeyEvent.VK_A, Event.CTRL_MASK));
        loadCharacter.setMnemonic( KeyEvent.VK_O );
        loadCharacter.setAccelerator(KeyStroke.getKeyStroke(
                    KeyEvent.VK_O, Event.CTRL_MASK));


        this.setJMenuBar( menuBar );
        this.addWindowListener( new CloseHook( closeHook ) );		

        levelEditor.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                String[] args = new String[0];
                com.rafkind.paintown.Editor.main(args);
            }
        });

        quit.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                System.exit(0);
            }
        });

        clearCache.addActionListener( new ActionListener(){
            public void actionPerformed(ActionEvent evt) {
                MaskedImage.clearCache();
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

        // Lets add our tabbedPane and some actions

        getContentPane().add(pane);

        closeTab.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event){
                pane.remove(CURRENT_TAB);
            }
        });

        pane.addChangeListener( new ChangeListener(){
            public void stateChanged(ChangeEvent changeEvent){
                JTabbedPane sourceTabbedPane = (JTabbedPane) changeEvent.getSource();
                int index = sourceTabbedPane.getSelectedIndex();
                CURRENT_TAB = index;
            }
        });

        newProjectile.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event){
                Projectile projectile = new Projectile( "new projectile" );
                ProjectilePane pane = new ProjectilePane( Animator.this, projectile );
                addNewTab( pane.getEditor(), projectile.getName() );
            }
        });

        newCharacter.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                CharacterStats character = new CharacterStats( "New Character" );
                Player pane = new Player( Animator.this, character );

                addNewTab( pane.getEditor(), "New Character" );
            }
        });

        openProjectile.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                JFileChooser chooser = new JFileChooser( new File( "." ) );	
                chooser.setFileFilter( new FileFilter(){
                    public boolean accept( File f ){
                        return f.isDirectory() || f.getName().endsWith( ".txt" );
                    }

                    public String getDescription(){
                        return "Projectile files";
                    }
                });

                int returnVal = chooser.showOpenDialog( Animator.this );
                if ( returnVal == JFileChooser.APPROVE_OPTION ){
                    final File f = chooser.getSelectedFile();
                    try{
                        Projectile projectile = new Projectile( f.getName(), f );
                        projectile.setPath( f );
                        ProjectilePane pane = new ProjectilePane( Animator.this, projectile );
                        addNewTab( pane.getEditor(), projectile.getName() );
                    } catch ( LoadException le ){
                        //showError( "Could not load " + f.getName() );
                        System.out.println( "Could not load " + f.getName() );
                        le.printStackTrace();
                    }
                }	
            }

        });

        loadCharacter.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                JFileChooser chooser = new JFileChooser( new File( "." ) );	
                chooser.setFileFilter( new FileFilter(){
                    public boolean accept( File f ){
                        return f.isDirectory() || f.getName().endsWith( ".txt" );
                    }

                    public String getDescription(){
                        return "Character files (*.txt)";
                    }
                });

                int returnVal = chooser.showOpenDialog( Animator.this );
                if ( returnVal == JFileChooser.APPROVE_OPTION ){
                    final File f = chooser.getSelectedFile();
                    try{
                        CharacterStats character = new CharacterStats( "New Character", f );
                        Player tempPlayer = new Player( Animator.this, character );
                        addNewTab( tempPlayer.getEditor(), character.getName() );
                    } catch ( LoadException le ){
                        //showError( "Could not load " + f.getName() );
                        System.out.println( "Could not load " + f.getName() );
                        le.printStackTrace();
                    }
                }	
            }
        });

        final Lambda2 saveObject = new Lambda2(){
            public Object invoke( Object obj, Object path_ ){
                BasicObject object = (BasicObject) obj;
                File path = (File) path_;

                object.setPath( path );
                try{
                    object.saveData();
                    doMessagePopup("Saved to " + path);
                } catch ( Exception e ){
                    doMessagePopup("Could not save:" + e.getMessage());
                    e.printStackTrace();
                }
                return null;
            }
        };

        saveProjectile.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                if ( pane.getSelectedComponent() != null ){
                    BasicObject object = ((SpecialPanel)pane.getSelectedComponent()).getObject();
                    if ( object != null ){

                        File file = object.getPath();
                        if ( file == null ){
                            file = userSelectFile();
                        }

                        /* write the text to a file */
                        if ( file != null ){
                            saveObject.invoke_( object, file );
                        }
                    }
                }
            }
        });

        saveCharacter.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                if ( pane.getSelectedComponent() != null ){
                    BasicObject object = ((SpecialPanel)pane.getSelectedComponent()).getObject();
                    if ( object != null ){

                        File file = object.getPath();
                        if ( file == null ){
                            file = userSelectFile();
                        }

                        /* write the text to a file */
                        if ( file != null ){
                            saveObject.invoke_( object, file );
                        }
                    }
                }
            }
        });

        saveCharacterAs.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                if ( pane.getSelectedComponent() != null ){
                    BasicObject object = ((SpecialPanel)pane.getSelectedComponent()).getObject();
                    if ( object != null ){
                        File file = userSelectFile();
                        /* write the text to a file */
                        if ( file != null ){
                            saveObject.invoke_( object, file );
                        }
                    }
                }
            }
        });
    }

    private void doMessagePopup(String message){
        Point here = this.getLocation();
        SwingUtilities.convertPointToScreen(here, this);
        int x = (int)(here.getX() + this.getWidth() / 3);
        int y = (int)(here.getY() + this.getHeight() / 3);
        JLabel label = new JLabel(message);
        label.setBackground(new Color(0,43,250));
        label.setBorder(BorderFactory.createEtchedBorder(EtchedBorder.LOWERED));
        final Popup popup = PopupFactory.getSharedInstance().getPopup(this, label, x, y );
        popup.show();
        final javax.swing.Timer kill = new javax.swing.Timer(1000, new ActionListener(){
            public void actionPerformed( ActionEvent event ){
            }
        });
        kill.addActionListener(new ActionListener(){
            public void actionPerformed( ActionEvent event ){
                popup.hide();
                kill.stop();
            }
        });
        kill.start();
    }

    private File userSelectFile(){
        JFileChooser chooser = new JFileChooser( new File( "." ) );
        int returnVal = chooser.showOpenDialog( Animator.this );
        if ( returnVal == JFileChooser.APPROVE_OPTION ){
            return chooser.getSelectedFile();
        } else {
            return null;
        }
    }

    public static File dataPath( File f ){
        return new File( getDataPath().getPath() + "/" + f.getPath() );
    }

    public static String dataPath( String s ){
        return getDataPath().getPath() + "/" + s;
    }

    public static File getDataPath(){
        return dataPath;
    }

    private static void setDataPath( File f ){
        dataPath = f;
    }

    public static RelativeFileChooser getNewFileChooser(){
        return new RelativeFileChooser( animator, getDataPath() );
    }

    public void addNewTab( SpecialPanel panel, String name ){
        pane.add( name, panel );

        pane.setSelectedIndex(pane.getTabCount()-1);

        CURRENT_TAB = pane.getSelectedIndex();

        final SpecialPanel tempPanel = (SpecialPanel)pane.getComponentAt(CURRENT_TAB);
        if ( tempPanel.getTextBox() != null ){

            panel.getTextBox().getDocument().addDocumentListener(new DocumentListener(){
                public void changedUpdate(DocumentEvent e){
                    pane.setTitleAt(pane.indexOfComponent(tempPanel),tempPanel.getTextBox().getText());
                }

                public void insertUpdate(DocumentEvent e){
                    pane.setTitleAt(pane.indexOfComponent(tempPanel),tempPanel.getTextBox().getText());
                }

                public void removeUpdate(DocumentEvent e){
                    pane.setTitleAt(pane.indexOfComponent(tempPanel),tempPanel.getTextBox().getText());
                }
            });
        }
    }

    public static void main(String [] args){
        try{
            final Animator animator = new Animator();
            SwingUtilities.invokeLater( new Runnable(){
                public void run(){
                    animator.setVisible( true );
                }
            });
        } catch (Exception e){
            System.err.println(e);
        }
    }
}
