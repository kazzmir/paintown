package com.rafkind.paintown.animator;

import java.util.*;
import java.awt.*;
import java.awt.image.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import java.io.*;
import java.util.regex.*;

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

    private static int CURRENT_TAB = 0;
    private static Animator animator;

    private static JTabbedPane pane = new JTabbedPane();

    public Animator() throws Exception {
        super("Paintown Animator");
        this.setSize( (int)Toolkit.getDefaultToolkit().getScreenSize().getWidth() - 100, (int)Toolkit.getDefaultToolkit().getScreenSize().getHeight() - 100 );

        Closer.open();

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
                Closer.close();
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
                com.rafkind.paintown.Editor2.main(args);
            }
        });

        quit.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent evt) {
                closeHook.invoke_();
            }
        });

        clearCache.addActionListener( new ActionListener(){
            public void actionPerformed(ActionEvent evt) {
                MaskedImage.clearCache();
            }
        });
        
        final SwingEngine quickEngine = new SwingEngine("animator/quick.xml");
        final JLabel quickDisplay = (JLabel) quickEngine.find("display");
        final Icon quickDisplayIcon = quickDisplay.getIcon();
        quickDisplay.setIcon(null);

        /* cant get the gif through normal means, so just hack it above */
        /*
        System.out.println("Gif = " + this.getClass().getResource("animator/quick.gif"));
        quickDisplay.setIcon(new ImageIcon(this.getClass().getResource("animator/quick.gif")));
        */

        class QuickCharacterLoaderModel implements ListModel {
            private List<File> data;
            private List listeners;
            private List filtered;
            private Pattern filter;

            public QuickCharacterLoaderModel(){
                listeners = new ArrayList();
                filtered = new ArrayList();
                load(Data.getDataPath());
                filter = Pattern.compile(".*");
            }

            void load(final File path){
                data = new ArrayList();
        
                SwingUtilities.invokeLater(new Runnable(){
                    public void run(){
                        quickDisplay.setIcon(quickDisplayIcon);
                    }
                });

                final Lambda1 loadMore = new Lambda1(){
                    int now = 1;

                    private synchronized int add(int x){
                        now += x;
                        return now;
                    }

                    public Object invoke(Object i_){
                        int x = (Integer) i_;
                        if (add(x) <= 0){
                            SwingUtilities.invokeLater(new Runnable(){
                                public void run(){
                                    quickDisplay.setIcon(null);
                                }
                            });
                        }
                        return null;
                    }
                };

                new Thread(new Runnable(){
                    public void run(){
                        FilenameFilter filter = new FilenameFilter(){
                            public boolean accept(File dir, String name){
                                String up = dir.getName();
                                // System.out.println("Maybe file " + up + "/" + name);
                                return !dir.getName().equals(".svn") &&
                                       (new File(dir, name).isDirectory() ||
                                       name.equals(up + ".txt"));
                            }
                        };

                        Animator.this.findPossibleFiles(path, filter, new Lambda1(){
                            public Object invoke(final Object f){
                                SwingUtilities.invokeLater(new Runnable(){
                                    public void run(){
                                        add((File) f);
                                    }
                                });
                                return null;
                            }
                        }, loadMore);
                    }
                }).start();
            }

            public void setFilter(String filter){
                this.filter = Pattern.compile(".*" + filter + ".*");
                updateView();
            }

            /* keep list sorted by modification time
             * possible optimization: binary search for place to insert
             */
            private void insert(File file){
                long time = file.lastModified();
                for (int i = 0; i < data.size(); i++){
                    File ok = (File) data.get(i);
                    if (time > ok.lastModified()){
                        data.add(i, file);
                        return;
                    }
                }
                data.add(file);
            }

            private void refilter(){
                filtered = new ArrayList();
                for (File file : data){
                    try{
                        Matcher m = filter.matcher(file.getCanonicalPath());
                        if (m.matches()){
                            filtered.add(file);
                        }
                    } catch (IOException e){
                        /* ignore */
                    }
                }
            }

            private void updateView(){
                refilter();
                ListDataEvent event = new ListDataEvent( this, ListDataEvent.INTERVAL_ADDED, filtered.size(), filtered.size() );
                for ( Iterator it = listeners.iterator(); it.hasNext(); ){
                    ListDataListener l = (ListDataListener) it.next();
                    l.intervalAdded(event);
                }
            }

            public synchronized void add(File file){
                insert(file);
                updateView();
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
                return filtered;
            }

            public void addListDataListener( ListDataListener l ){
                listeners.add( l );
            }

            public Object getElementAt( int index ){
                return this.filtered.get(index);
            }

            public int getSize(){
                return this.filtered.size();
            }

            public void removeListDataListener( ListDataListener l ){
                this.listeners.remove(l);
            }
        }

        final QuickCharacterLoaderModel quickLoaderModel = new QuickCharacterLoaderModel();

        final JTextField quickFilter = (JTextField) quickEngine.find("filter");
        quickFilter.getDocument().addDocumentListener(new DocumentListener(){
            public void changedUpdate(DocumentEvent e){
                quickLoaderModel.setFilter(quickFilter.getText());
            }

            public void insertUpdate(DocumentEvent e){
                quickLoaderModel.setFilter(quickFilter.getText());
            }

            public void removeUpdate(DocumentEvent e){
                quickLoaderModel.setFilter(quickFilter.getText());
            }
        });

        // final JList quickLoader = new JList(quickLoaderModel);
        final JList quickLoader = (JList) quickEngine.find("list");

        final Lambda0 quickDoLoad = new Lambda0(){
            public Object invoke(){
                /*
                Object[] files = (Object[]) quickLoader.getSelectedValues();
                for (Object file_ : files){
                    final File file = (File) file_;
                    new Thread(new Runnable(){
                        public void run(){
                            try{
                                CharacterStats character = new CharacterStats("", file);
                                Player tempPlayer = new Player(Animator.this, character);
                                addNewTab(tempPlayer.getEditor(), character.getName());
                            } catch (LoadException le){
                                //showError( "Could not load " + f.getName() );
                                System.out.println( "Could not load " + file.getName() );
                                le.printStackTrace();
                            }
                        }
                    }).start();
                }
                */

                return null;
            }
        };

        quickLoader.setModel(quickLoaderModel);
        quickLoader.addMouseListener(new MouseAdapter() {
            public void mouseClicked(MouseEvent e) {
                if (e.getClickCount() == 2){
                    quickDoLoad.invoke_();
                }
            }
        });

        quickLoader.getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_ENTER, 0), "open");
        quickLoader.getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_A, 2), "select-all");
        quickLoader.getActionMap().put("open", new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                quickDoLoad.invoke_();
            }
        });
        
        quickLoader.getActionMap().put("select-all", new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                int[] indicies = new int[quickLoaderModel.getSize()];
                for (int i = 0; i < quickLoaderModel.getSize(); i++){
                    indicies[i] = i;
                }
                quickLoader.setSelectedIndices(indicies);
            }
        });

        JButton quickLoadButton = (JButton) quickEngine.find("load");
        quickLoadButton.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent event){
                quickDoLoad.invoke_();
            }
        });
        
        // pane.add("Quick character loader", new JScrollPane(quickLoader));
        pane.add("Quick character loader", (JPanel) quickEngine.getRootComponent());

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
                box.set( Data.getDataPath() );
                path.setText( Data.getDataPath().getPath() );
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
                        File path = (File) box.get();
                        Data.setDataPath(path);
                        quickLoaderModel.load(path);
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
                if (CURRENT_TAB > 0 && CURRENT_TAB < pane.getTabCount()){
                    pane.remove(CURRENT_TAB);
                }
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
                /*
                Projectile projectile = new Projectile( "new projectile" );
                ProjectilePane pane = new ProjectilePane( Animator.this, projectile );
                addNewTab( pane.getEditor(), projectile.getName() );
                */
            }
        });

        newCharacter.addActionListener( new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                /*
                CharacterStats character = new CharacterStats( "New Character" );
                Player pane = new Player( Animator.this, character );

                addNewTab( pane.getEditor(), "New Character" );
                */
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
                    /*
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
                    */
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
                    /*
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
                    */
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

    private void findPossibleFiles(final File directory, final FilenameFilter filter, final Lambda1 found, final Lambda1 loading){
        File[] files = directory.listFiles(filter);
        for (final File file : files){
            if (file.isDirectory()){
                loading.invoke_(1);
                new Thread(new Runnable(){
                    public void run(){
                        findPossibleFiles(file, filter, found, loading);
                    }
                }).start();
            } else {
                found.invoke_(file);
            }
        }
        loading.invoke_(-1);
    }

    public static File dataPath( File f ){
        return new File( Data.getDataPath().getPath() + "/" + f.getPath() );
    }

    public static String dataPath( String s ){
        return Data.getDataPath().getPath() + "/" + s;
    }

    public static RelativeFileChooser getNewFileChooser(){
        return new RelativeFileChooser(animator, Data.getDataPath());
    }

    public void addNewTab(final SpecialPanel panel, final String name){
        SwingUtilities.invokeLater(new Runnable(){
            public void run(){
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
        });
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
