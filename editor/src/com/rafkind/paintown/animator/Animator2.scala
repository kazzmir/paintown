package com.rafkind.paintown.animator

import java.io._
import java.awt
import javax.swing
import java.util.regex.Pattern

import org.swixml.SwingEngine;
import com.rafkind.paintown.exception._

import com.rafkind.paintown._

class NewAnimator extends swing.JFrame("Paintown Animator"){

  def get[T](list:List[T], index:Int):T = {
    list.find(list.indexOf(_) == index) match {
      case Some(obj) => obj
      case None => throw new Exception("failed to find " + index)
    }
  }

  def toScalaList[T](list:java.util.List[T]):List[T] = {
    var out:List[T] = List[T]()
    for (item <- scala.collection.JavaConversions.asScalaBuffer(list)){
      out = out :+ item
    }
    out
  }

  def construct(){
    val screen = awt.Toolkit.getDefaultToolkit().getScreenSize()
    this.setSize((screen.getWidth() * 0.9).toInt,
                 (screen.getHeight() * 0.9).toInt);

    Closer.open()

    val menuBar = new swing.JMenuBar();
    val menuProgram = new swing.JMenu("Program");
    val levelEditor = new swing.JMenuItem("Run the level editor");
    val quit = new swing.JMenuItem("Quit");
    val clearCache = new swing.JMenuItem("Clear image cache");
    val data = new swing.JMenuItem("Data path");
    val closeTab = new swing.JMenuItem("Close Tab");
    menuProgram.add(levelEditor);
    menuProgram.add(data);
    menuProgram.add(clearCache);
    menuProgram.add(closeTab);
    menuProgram.add(quit);
    menuBar.add(menuProgram);

    val menuProjectile = new swing.JMenu("Projectile");
    menuBar.add(menuProjectile);

    val newProjectile = new swing.JMenuItem("New Projectile");
    menuProjectile.add(newProjectile);
    val openProjectile = new swing.JMenuItem("Open Projectile");
    menuProjectile.add(openProjectile);
    val saveProjectile = new swing.JMenuItem("Save Projectile");
    menuProjectile.add(saveProjectile);

    val menuCharacter = new swing.JMenu("Character");
    menuBar.add(menuCharacter);

    val newCharacter = new swing.JMenuItem("New Character");
    menuCharacter.add(newCharacter);
    val loadCharacter = new swing.JMenuItem("Open Character");
    menuCharacter.add(loadCharacter);
    val saveCharacter = new swing.JMenuItem("Save Character");
    menuCharacter.add(saveCharacter);
    val saveCharacterAs = new swing.JMenuItem("Save Character As");
    menuCharacter.add(saveCharacterAs);

    menuProgram.setMnemonic(awt.event.KeyEvent.VK_P);
    data.setMnemonic(awt.event.KeyEvent.VK_D);
    closeTab.setMnemonic(awt.event.KeyEvent.VK_C);
    closeTab.setAccelerator(swing.KeyStroke.getKeyStroke(awt.event.KeyEvent.VK_W, awt.Event.CTRL_MASK));
    quit.setMnemonic(awt.event.KeyEvent.VK_Q);
    quit.setAccelerator(swing.KeyStroke.getKeyStroke(awt.event.KeyEvent.VK_Q, awt.Event.CTRL_MASK));
    newCharacter.setMnemonic(awt.event.KeyEvent.VK_N);
    newCharacter.setAccelerator(swing.KeyStroke.getKeyStroke(awt.event.KeyEvent.VK_N, awt.Event.CTRL_MASK));
    menuCharacter.setMnemonic(awt.event.KeyEvent.VK_H);
    saveCharacter.setMnemonic(awt.event.KeyEvent.VK_S);
    saveCharacter.setAccelerator(swing.KeyStroke.getKeyStroke(awt.event.KeyEvent.VK_S, awt.Event.CTRL_MASK));
    saveCharacterAs.setMnemonic(awt.event.KeyEvent.VK_A);
    saveCharacterAs.setAccelerator(swing.KeyStroke.getKeyStroke(awt.event.KeyEvent.VK_A, awt.Event.CTRL_MASK));
    loadCharacter.setMnemonic(awt.event.KeyEvent.VK_O);
    loadCharacter.setAccelerator(swing.KeyStroke.getKeyStroke(awt.event.KeyEvent.VK_O, awt.Event.CTRL_MASK));

    this.setJMenuBar(menuBar);
    this.addWindowListener(new awt.event.WindowAdapter(){
      override def windowClosing(e:awt.event.WindowEvent){
        Closer.close();
      }
    });

    levelEditor.addActionListener(new swing.AbstractAction(){
      override def actionPerformed(event:awt.event.ActionEvent){
        com.rafkind.paintown.Editor2.main(new Array[String](0));
      }
    });

    quit.addActionListener(new awt.event.ActionListener() {
      override def actionPerformed(event:awt.event.ActionEvent){
        Closer.close()
      }
    });

    clearCache.addActionListener(new awt.event.ActionListener(){
      override def actionPerformed(event:awt.event.ActionEvent){
        MaskedImage.clearCache();
      }
    });

    val quickEngine = new SwingEngine("animator/quick.xml");
    val quickDisplay = quickEngine.find("display").asInstanceOf[swing.JLabel];
    val quickDisplayIcon = quickDisplay.getIcon();
    // quickDisplay.setIcon(quickDisplayIcon);

    class QuickCharacterLoaderModel extends swing.ListModel {
      var listeners:List[swing.event.ListDataListener] = List[swing.event.ListDataListener]()
      var data:List[File] = List[File]()
      var filtered:List[File] = List[File]()
      var filter:Pattern = Pattern.compile(".*")

      load(Data.getDataPath())

      def load(path:File){
        val self = this
        data = List[File]()
        new swing.SwingWorker[List[File], File]{
          override def doInBackground():List[File] = {
            val filter = new FilenameFilter(){
              override def accept(dir:File, name:String):Boolean = {
                val up = dir.getName();
                // System.out.println("Maybe file " + up + "/" + name);
                return !dir.getName().equals(".svn") &&
                (new File(dir, name).isDirectory() ||
                  name.equals(up + ".txt"));
              }
            };
            
            def find(directory:File){
              val files = directory.listFiles(filter);
              for (file <- files){
                if (file.isDirectory()){
                  find(file)
                } else {
                  publish(file)
                }
              }
            }

            find(path)
            return List[File]()
          }

          override def done(){
            quickDisplay.setIcon(null)
          }

          override def process(files:java.util.List[File]) = {
            self.add(toScalaList(files))
          }
        }.execute()
      }

      def modificationCompare(file1:File, file2:File):Boolean = {
        file1.lastModified() < file2.lastModified()
      }

      def add(files:List[File]){
        data = data ++ files
        updateView(filter)
        /*
        filtered = (filtered ++ files).sort(modificationCompare)
        val event = new swing.event.ListDataEvent(this, swing.event.ListDataEvent.INTERVAL_ADDED, 0, filtered.size);
        for (listener <- listeners){
          listener.intervalAdded(event);
        }
        */
      }

    /*
      val List[File] data;
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


            / * keep list sorted by modification time
             * possible optimization: binary search for place to insert
             * /
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
                        / * ignore * /
                    }
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
            */

           def refilter(pattern:Pattern):List[File] = {
             data.filter((file) => pattern.matcher(file.getCanonicalPath()).matches).sort(modificationCompare)
           }

           def updateView(filter:Pattern){
                this.filtered = refilter(filter);
                val event = new swing.event.ListDataEvent(this, swing.event.ListDataEvent.INTERVAL_ADDED, 0, filtered.size);
                for (listener <- listeners){
                  listener.intervalAdded(event);
                }
            }

            override def addListDataListener(listener:swing.event.ListDataListener){
              listeners = listeners :+ listener
            }

            override def getElementAt(index:Int):Object = {
                return get(filtered, index)
                // return this.filtered.get(index);
            }

            override def getSize():Int = {
                this.filtered.size
            }

            override def removeListDataListener(listener:swing.event.ListDataListener){
                listeners = listeners - listener
            }

            def setFilter(input:String){
                this.filter = Pattern.compile(".*" + input + ".*");
                updateView(filter);
            }
        }

        val quickLoaderModel = new QuickCharacterLoaderModel();

        val quickFilter = quickEngine.find("filter").asInstanceOf[swing.JTextField];
        quickFilter.getDocument().addDocumentListener(new swing.event.DocumentListener(){
            override def changedUpdate(event:swing.event.DocumentEvent){
                quickLoaderModel.setFilter(quickFilter.getText());
            }

            override def insertUpdate(event:swing.event.DocumentEvent){
                quickLoaderModel.setFilter(quickFilter.getText());
            }

            override def removeUpdate(event:swing.event.DocumentEvent){
                quickLoaderModel.setFilter(quickFilter.getText());
            }
        });

        val quickLoader = quickEngine.find("list").asInstanceOf[swing.JList];

        def quickDoLoad(){
          val files = quickLoader.getSelectedValues().asInstanceOf[Array[Object]];
          for (file_ <- files){
            val file = file_.asInstanceOf[File]
            new Thread(new Runnable(){
              def run(){
                try{
                  /*
                  CharacterStats character = new CharacterStats("", file);
                  Player tempPlayer = new Player(NewAnimator.this, character);
                  SwingUtilities.invokeLater(new Runnable(){
                    def run(){
                      addNewTab(tempPlayer.getEditor(), character.getName());
                    }
                  }
                  */
                } catch {
                  case le:LoadException => {
                    //showError( "Could not load " + f.getName() );
                    System.out.println( "Could not load " + file.getName() );
                    le.printStackTrace();
                  }
                }
              }
             }).start();
            }
          }

        quickLoader.setModel(quickLoaderModel);
        quickLoader.addMouseListener(new awt.event.MouseAdapter(){
            override def mouseClicked(event:awt.event.MouseEvent){
                if (event.getClickCount() == 2){
                    // quickDoLoad.invoke_();
                }
            }
        });

        quickLoader.getInputMap().put(swing.KeyStroke.getKeyStroke(awt.event.KeyEvent.VK_ENTER, 0), "open");
        quickLoader.getInputMap().put(swing.KeyStroke.getKeyStroke(awt.event.KeyEvent.VK_A, 2), "select-all");
        quickLoader.getActionMap().put("open", new swing.AbstractAction(){
            override def actionPerformed(event:awt.event.ActionEvent){
                // quickDoLoad.invoke_();
            }
        });
        
        quickLoader.getActionMap().put("select-all", new swing.AbstractAction(){
            override def actionPerformed(event:awt.event.ActionEvent){
              /*
                int[] indicies = new int[quickLoaderModel.getSize()];
                for (int i = 0; i < quickLoaderModel.getSize(); i++){
                    indicies[i] = i;
                }
                quickLoader.setSelectedIndices(indicies);
                */
            }
        });

        val quickLoadButton = quickEngine.find("load").asInstanceOf[swing.JButton];
        quickLoadButton.addActionListener(new swing.AbstractAction(){
            override def actionPerformed(event:awt.event.ActionEvent){
                // quickDoLoad.invoke_();
            }
        });

        val pane = new swing.JTabbedPane()
        
        // pane.add("Quick character loader", new JScrollPane(quickLoader));
        pane.add("Quick character loader", quickEngine.getRootComponent().asInstanceOf[swing.JPanel]);

        getContentPane().add(pane)

        data.addActionListener(new awt.event.ActionListener(){
          override def actionPerformed(event:awt.event.ActionEvent){
                /* just a container for an object */
                class ObjectBox{
                  var internal:Object = null
                  def set(o:Object){
                    internal = o
                  }

                  def get():Object = {
                    return internal;
                  }
                }
                val engine = new SwingEngine("data-path.xml");
                val path = engine.find("path").asInstanceOf[swing.JTextField];
                val box = new ObjectBox();
                box.set(Data.getDataPath());
                path.setText( Data.getDataPath().getPath() );
                val change = engine.find("change").asInstanceOf[swing.JButton];
                change.addActionListener(new swing.AbstractAction(){
                  override def actionPerformed(event:awt.event.ActionEvent){
                    val chooser = new swing.JFileChooser(new File("."));	
                    chooser.setFileSelectionMode(swing.JFileChooser.DIRECTORIES_ONLY);
                    val returnVal = chooser.showOpenDialog(NewAnimator.this);
                    if (returnVal == swing.JFileChooser.APPROVE_OPTION){
                      val newPath:File = chooser.getSelectedFile();
                      path.setText(newPath.getPath());
                      box.set(newPath);
                    }
                  }
                });
                val save = engine.find("save").asInstanceOf[swing.JButton];
                val cancel = engine.find("cancel").asInstanceOf[swing.JButton];
                val dialog = new swing.JDialog(NewAnimator.this, "Paintown data path");
                save.addActionListener(new swing.AbstractAction(){
                  override def actionPerformed(event:awt.event.ActionEvent){
                    val path = box.get().asInstanceOf[File];
                    Data.setDataPath(path);
                    quickLoaderModel.load(path);
                    dialog.setVisible(false);
                  }
                });

                cancel.addActionListener(new swing.AbstractAction(){
                  override def actionPerformed(event:awt.event.ActionEvent){
                    dialog.setVisible(false);
                  }
                });

                val panel = engine.getRootComponent().asInstanceOf[swing.JPanel];
                dialog.getContentPane().add(panel);
                dialog.setSize(300, 300);
                dialog.setVisible(true);
            }
        });

        // Lets add our tabbedPane and some actions

        /*
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

                        / * write the text to a file * /
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

                        / * write the text to a file * /
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
                        / * write the text to a file * /
                        if ( file != null ){
                            saveObject.invoke_( object, file );
                        }
                    }
                }
            }
        });
        */
  }

  construct()
}

object Animator2{
  def main(args: Array[String]):Unit = {
    val editor = new NewAnimator();
    swing.SwingUtilities.invokeLater(new Runnable(){
        def run(){
          editor.setVisible(true);
        }
      });
  }
}
