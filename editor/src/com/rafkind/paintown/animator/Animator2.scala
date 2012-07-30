package com.rafkind.paintown.animator

import java.io._
import java.awt
import javax.swing
import java.util.regex.Pattern

import org.swixml.SwingEngine;
import com.rafkind.paintown.exception._

import com.rafkind.paintown._

class DrawPropertiesListener {
  def updateBackgroundColor(color:awt.Color) = {
  }
}

/* putting this class here for now, but it should probably be somewhere else */
class DrawProperties {
  var color:awt.Color = new awt.Color(0, 0, 0)
  var listeners:List[DrawPropertiesListener] = List()

  def setBackgroundColor(newColor:awt.Color) = {
    color = newColor
    for (listener <- listeners){
      listener.updateBackgroundColor(color)
    }
  }

  def getBackgroundColor():awt.Color = color

  def addListener(listener:DrawPropertiesListener){
    listeners = listeners :+ listener
  }

  def removeListener(listener:DrawPropertiesListener){
    // listeners = listeners :- listener
  }
}

class NewAnimator extends swing.JFrame("Paintown Animator"){
      
  val propertyLastLoaded = "animator:last-loaded"
  /* Define it here so we can modify it in loadPlayer */
  val loadLastCharacter = new swing.JMenuItem("Load last character")

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

  val pane = new swing.JTabbedPane()

  def construct(){
    val screen = awt.Toolkit.getDefaultToolkit().getScreenSize()
    this.setSize((screen.getWidth() * 0.9).toInt,
                 (screen.getHeight() * 0.9).toInt);

    Closer.open()

    val menuBar = new swing.JMenuBar()
    val menuProgram = new swing.JMenu("Program")
    val undo = new swing.JMenuItem("Undo last action");
    // val redo = new swing.JMenuItem("Redo last undo");
    val levelEditor = new swing.JMenuItem("Run the level editor")
    val quit = new swing.JMenuItem("Quit")
    val clearCache = new swing.JMenuItem("Clear image cache")
    val data = new swing.JMenuItem("Data path")
    val gameSpeed = new swing.JMenuItem("Game ticks")
    val closeTab = new swing.JMenuItem("Close Tab")
    menuProgram.add(levelEditor)
    menuProgram.add(undo)
    // menuProgram.add(redo)
    menuProgram.add(data)
    menuProgram.add(gameSpeed)
    menuProgram.add(clearCache)
    menuProgram.add(closeTab)
    menuProgram.add(quit)
    menuBar.add(menuProgram)

    Undo.setUndoMenuItem(undo)

    val menuProjectile = new swing.JMenu("Projectile")
    menuBar.add(menuProjectile)

    val newProjectile = new swing.JMenuItem("New Projectile")
    menuProjectile.add(newProjectile)
    val openProjectile = new swing.JMenuItem("Open Projectile")
    menuProjectile.add(openProjectile)
    val saveProjectile = new swing.JMenuItem("Save Projectile")
    menuProjectile.add(saveProjectile)

    val menuCharacter = new swing.JMenu("Character")
    menuBar.add(menuCharacter)

    val newCharacter = new swing.JMenuItem("New Character")
    menuCharacter.add(newCharacter)
    val loadCharacter = new swing.JMenuItem("Open Character")
    menuCharacter.add(loadCharacter)
    menuCharacter.add(loadLastCharacter)
    val saveCharacter = new swing.JMenuItem("Save Character")
    menuCharacter.add(saveCharacter)
    val saveCharacterAs = new swing.JMenuItem("Save Character As")
    menuCharacter.add(saveCharacterAs)

    undo.setMnemonic(awt.event.KeyEvent.VK_U)
    undo.setAccelerator(swing.KeyStroke.getKeyStroke(awt.event.KeyEvent.VK_Z, awt.Event.CTRL_MASK))
    
    // redo.setMnemonic(awt.event.KeyEvent.VK_R)
    // redo.setAccelerator(swing.KeyStroke.getKeyStroke(awt.event.KeyEvent.VK_R, awt.Event.CTRL_MASK))

    menuProgram.setMnemonic(awt.event.KeyEvent.VK_P)
    data.setMnemonic(awt.event.KeyEvent.VK_D)
    closeTab.setMnemonic(awt.event.KeyEvent.VK_C)
    closeTab.setAccelerator(swing.KeyStroke.getKeyStroke(awt.event.KeyEvent.VK_W, awt.Event.CTRL_MASK))
    quit.setMnemonic(awt.event.KeyEvent.VK_Q)
    quit.setAccelerator(swing.KeyStroke.getKeyStroke(awt.event.KeyEvent.VK_Q, awt.Event.CTRL_MASK))
    newCharacter.setMnemonic(awt.event.KeyEvent.VK_N)
    newCharacter.setAccelerator(swing.KeyStroke.getKeyStroke(awt.event.KeyEvent.VK_N, awt.Event.CTRL_MASK))
    menuCharacter.setMnemonic(awt.event.KeyEvent.VK_H)
    saveCharacter.setMnemonic(awt.event.KeyEvent.VK_S)
    saveCharacter.setAccelerator(swing.KeyStroke.getKeyStroke(awt.event.KeyEvent.VK_S, awt.Event.CTRL_MASK))
    saveCharacterAs.setMnemonic(awt.event.KeyEvent.VK_A)
    saveCharacterAs.setAccelerator(swing.KeyStroke.getKeyStroke(awt.event.KeyEvent.VK_A, awt.Event.CTRL_MASK))
    loadCharacter.setMnemonic(awt.event.KeyEvent.VK_O)
    loadCharacter.setAccelerator(swing.KeyStroke.getKeyStroke(awt.event.KeyEvent.VK_O, awt.Event.CTRL_MASK))

    this.setJMenuBar(menuBar)
    this.addWindowListener(new awt.event.WindowAdapter(){
      override def windowClosing(e:awt.event.WindowEvent){
        Closer.close();
      }
    })
    
    Config.getConfig().get(propertyLastLoaded).asInstanceOf[String] match {
      case null => {}
      case what => loadLastCharacter.setText("Load last character " + new File(what).getName())
    }

    loadLastCharacter.addActionListener(new swing.AbstractAction(){
      override def actionPerformed(event:awt.event.ActionEvent){
        val path = Config.getConfig().get(propertyLastLoaded).asInstanceOf[String]
        if (path != null){
          loadPlayer(new File(path))
        }
      }
    })

    undo.addActionListener(new swing.AbstractAction(){
      override def actionPerformed(event:awt.event.ActionEvent){
        Undo.popUndo()
      }
    })

    /*
    redo.addActionListener(new swing.AbstractAction(){
      override def actionPerformed(event:awt.event.ActionEvent){
        Undo.popRedo()
      }
    })
    */

    levelEditor.addActionListener(new swing.AbstractAction(){
      override def actionPerformed(event:awt.event.ActionEvent){
        com.rafkind.paintown.level.Editor2.main(new Array[String](0));
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
      var listeners:List[swing.event.ListDataListener] = List()
      var data:List[File] = List()
      var filtered:List[File] = List()
      var filter:Pattern = Pattern.compile(".*")

      load(Data.getDataPath())

      def load(path:File){
        val self = this
        data = List()
        new swing.SwingWorker[List[File], File]{
          override def doInBackground():List[File] = {
            val filter = new FilenameFilter(){
              override def accept(dir:File, name:String):Boolean = {
                val up = dir.getName();
                // System.out.println("Maybe file " + up + "/" + name);
                return !dir.getName().equals(".svn") &&
                (new File(dir, name).isDirectory() ||
                  name.endsWith(".txt"));
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
            return List()
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
      }

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
                if (isPlayerFile(file)){
                  loadPlayer(file)
                } else if (isProjectileFile(file)){
                  loadProjectile(file)
                }
              }
             }).start();
            }
          }

        quickLoader.setModel(quickLoaderModel);
        quickLoader.addMouseListener(new awt.event.MouseAdapter(){
            override def mouseClicked(event:awt.event.MouseEvent){
                if (event.getClickCount() == 2){
                    quickDoLoad()
                }
            }
        });

        quickLoader.getInputMap().put(swing.KeyStroke.getKeyStroke(awt.event.KeyEvent.VK_ENTER, 0), "open");
        quickLoader.getInputMap().put(swing.KeyStroke.getKeyStroke(awt.event.KeyEvent.VK_A, 2), "select-all");
        quickLoader.getActionMap().put("open", new swing.AbstractAction(){
            override def actionPerformed(event:awt.event.ActionEvent){
                quickDoLoad()
            }
        });
        
        quickLoader.getActionMap().put("select-all", new swing.AbstractAction(){
            override def actionPerformed(event:awt.event.ActionEvent){
              val indicies:Array[Int] = new Array[Int](quickLoaderModel.getSize())
              for (index <- 0 to (quickLoaderModel.getSize() - 1)){
                indicies(index) = index
              }
              quickLoader.setSelectedIndices(indicies);
            }
        });

        val quickLoadButton = quickEngine.find("load").asInstanceOf[swing.JButton];
        quickLoadButton.addActionListener(new swing.AbstractAction(){
            override def actionPerformed(event:awt.event.ActionEvent){
                quickDoLoad()
            }
        });

        val quickLoaderPane = quickEngine.getRootComponent().asInstanceOf[swing.JPanel]
        
        pane.add("Quick character loader", quickLoaderPane)

        getContentPane().add(pane)

        gameSpeed.addActionListener(new awt.event.ActionListener(){
          override def actionPerformed(event:awt.event.ActionEvent){
            val speed = new swing.JDialog(NewAnimator.this, "Change game speed");
            speed.setSize(200, 100);
            val engine = new SwingEngine("animator/game-speed.xml");
            val spinner = engine.find("speed").asInstanceOf[swing.JSpinner]

            spinner.setValue(new Integer(NewAnimator.getTicksPerSecond()));
            spinner.addChangeListener(new swing.event.ChangeListener(){
                override def stateChanged(event:swing.event.ChangeEvent){
                  NewAnimator.setTicksPerSecond(spinner.getValue().asInstanceOf[Integer].intValue())
                }
            });

            speed.getContentPane().add(engine.getRootComponent().asInstanceOf[swing.JPanel])
            speed.setVisible(true)
          }
        })

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

        closeTab.addActionListener(new swing.AbstractAction(){
          override def actionPerformed(event:awt.event.ActionEvent){
            if (pane.getSelectedComponent() != quickLoaderPane){
              pane.remove(pane.getSelectedComponent())
            }
          }
        });

        /* dont need this
        pane.addChangeListener(new swing.event.ChangeListener(){
          override def stateChanged(changeEvent:swing.event.ChangeEvent){
                val sourceTabbedPane = changeEvent.getSource().asInstanceOf[JTabbedPane];
                val index = sourceTabbedPane.getSelectedIndex();
                CURRENT_TAB = index;
            }
        });
        */

        newProjectile.addActionListener(new swing.AbstractAction(){
          override def actionPerformed(event:awt.event.ActionEvent){
                val projectile = new Projectile("New Projectile");
                val pane = new ProjectilePane(NewAnimator.this, projectile);
                addNewTab(pane.getEditor(), projectile.getName());
            }
        });

        newCharacter.addActionListener(new swing.AbstractAction(){
          override def actionPerformed(event:awt.event.ActionEvent){
            val character = new CharacterStats("New Character");
            val pane = new Player(NewAnimator.this, character);

            addNewTab(pane.getEditor(), "New Character");
          }
        });

        openProjectile.addActionListener(new swing.AbstractAction(){
          override def actionPerformed(event:awt.event.ActionEvent){
            val chooser = new swing.JFileChooser(new File("."));	
            chooser.setFileFilter(new swing.filechooser.FileFilter(){
              override def accept(file:File):Boolean = {
                file.isDirectory() || file.getName().endsWith( ".txt" );
              }

              override def getDescription():String = {
                "Projectile files";
              }
            });

            val returnVal = chooser.showOpenDialog(NewAnimator.this);
            if (returnVal == swing.JFileChooser.APPROVE_OPTION){
              val file = chooser.getSelectedFile();
              try{
                val projectile = new Projectile(file.getName(), file);
                projectile.setPath(file);
                val pane = new ProjectilePane(NewAnimator.this, projectile);
                addNewTab(pane.getEditor(), projectile.getName());
              } catch {
                case fail:LoadException => {
                  //showError( "Could not load " + f.getName() );
                  println("Could not load" + file.getName())
                  fail.printStackTrace();
                }
              }	
            }
          }
        });

        loadCharacter.addActionListener(new swing.AbstractAction(){
          override def actionPerformed(event:awt.event.ActionEvent){
            val chooser = new swing.JFileChooser(new File("."));	
            chooser.setFileFilter(new swing.filechooser.FileFilter(){
              override def accept(file:File):Boolean = {
                file.isDirectory() || file.getName().endsWith(".txt");
              }

              override def getDescription():String = {
                return "Character files (*.txt)";
              }
            });

            val returnVal = chooser.showOpenDialog(NewAnimator.this);
            if (returnVal == swing.JFileChooser.APPROVE_OPTION){
              val file = chooser.getSelectedFile();
              loadPlayer(file)
              /*
              try{
                val character = new CharacterStats("New Character", file);
                val tempPlayer = new Player(NewAnimator.this, character);
                addNewTab(tempPlayer.getEditor(), character.getName());
              } catch {
                case fail:LoadException => {
                  //showError( "Could not load " + f.getName() );
                  println("Could not load " + file.getName());
                  fail.printStackTrace();
                }
              }	
              */
            }
          }
        });

        def saveObject(obj:BasicObject, path:File){
          obj.setPath(path);
          try{
            obj.saveData();
            doMessagePopup("Saved to " + path);
          } catch {
            case fail:Exception => {
              doMessagePopup("Could not save:" + fail.getMessage());
              println(fail)
            }
          }
        }

        def doSave(forceSelect:Boolean){
          if (pane.getSelectedComponent() != null){
            val basic = pane.getSelectedComponent().asInstanceOf[SpecialPanel].getObject()
            if (basic != null){
              var file:File = null
              if (!forceSelect){
                file = basic.getPath()
              }
              if (file == null){
                file = userSelectFile()
              }

              /* write the text to a file */
              if (file != null){
                saveObject(basic, file)
              }
            }
          }
        }

        saveProjectile.addActionListener(new swing.AbstractAction(){
          override def actionPerformed(event:awt.event.ActionEvent){
            doSave(false)
         }
        })

        saveCharacter.addActionListener(new swing.AbstractAction(){
          override def actionPerformed(event:awt.event.ActionEvent){
            doSave(false)
          }
        });

        saveCharacterAs.addActionListener(new swing.AbstractAction(){
          override def actionPerformed(event:awt.event.ActionEvent){
            doSave(true)
          }
        });
  }

  construct()

  def isPlayerFile(file:File):Boolean = {
    try{
      val reader = new TokenReader(file)
      return reader.nextToken().getName() == "character"
    } catch {
      case le:LoadException => {
        return false
      }
    }
  }

  def isProjectileFile(file:File):Boolean = {
    try{
      val reader = new TokenReader(file)
      return reader.nextToken().getName() == "projectile"
    } catch {
      case le:LoadException => {
        return false
      }
    }
  }

  def loadProjectile(file:File) = {
    try{
      println("Loading projectile " + file)
      val projectile = new Projectile(file.getName(), file)
      val pane = new ProjectilePane(NewAnimator.this, projectile);
      swing.SwingUtilities.invokeLater(new Runnable(){
        def run(){
          addNewTab(pane.getEditor(), projectile.getName());
        }
      })
     } catch {
       case le:LoadException => {
         //showError( "Could not load " + f.getName() );
         System.out.println("Could not load " + file.getName());
         le.printStackTrace();
       }
     }
  }

  def loadPlayer(file:File) = {
    try{
      println("Loading character " + file)
      val character = new CharacterStats("", file);
      val tempPlayer = new Player(NewAnimator.this, character);
      Config.getConfig().set(propertyLastLoaded, file.getAbsolutePath())
      loadLastCharacter.setText("Load last character " + file.getName())
      swing.SwingUtilities.invokeLater(new Runnable(){
        def run(){
          addNewTab(tempPlayer.getEditor(), character.getName());
        }
      })
     } catch {
       case le:LoadException => {
         //showError( "Could not load " + f.getName() );
         System.out.println("Could not load " + file.getName());
         le.printStackTrace();
       }
     }
  }

  def addNewTab(panel:SpecialPanel, name:String){
    swing.SwingUtilities.invokeLater(new Runnable(){
      override def run(){
        pane.add(name, panel);
        pane.setSelectedIndex(pane.getTabCount()-1);

        val tempPanel = panel
        if (tempPanel.getTextBox() != null){
          panel.getTextBox().getDocument().addDocumentListener(new swing.event.DocumentListener(){
            override def changedUpdate(event:swing.event.DocumentEvent){
              pane.setTitleAt(pane.indexOfComponent(tempPanel), tempPanel.getTextBox().getText());
            }

            override def insertUpdate(event:swing.event.DocumentEvent){
              pane.setTitleAt(pane.indexOfComponent(tempPanel), tempPanel.getTextBox().getText());
            }

            override def removeUpdate(event:swing.event.DocumentEvent){
              pane.setTitleAt(pane.indexOfComponent(tempPanel), tempPanel.getTextBox().getText());
            }
          });
         }
       }
    });
  }

  def doMessagePopup(message:String){
    val here = this.getLocation();
    swing.SwingUtilities.convertPointToScreen(here, this)
    val x = (here.getX() + this.getWidth() / 3).toInt
    val y = (here.getY() + this.getHeight() / 3).toInt
    val label = new swing.JLabel(message);
    label.setBackground(new awt.Color(0,43,250));
    label.setBorder(swing.BorderFactory.createEtchedBorder(swing.border.EtchedBorder.LOWERED));
    val popup = swing.PopupFactory.getSharedInstance().getPopup(this, label, x, y );
    popup.show();
    val kill = new swing.Timer(1000, new awt.event.ActionListener(){
      override def actionPerformed(event:awt.event.ActionEvent){
      }
    });
    kill.addActionListener(new awt.event.ActionListener(){
      override def actionPerformed(event:awt.event.ActionEvent){
        popup.hide();
        kill.stop();
      }
    });
    kill.start();
  }

  def userSelectFile():File = {
    val chooser = new swing.JFileChooser(new File("."))
    val returnVal = chooser.showOpenDialog(NewAnimator.this)
    if (returnVal == swing.JFileChooser.APPROVE_OPTION){
      chooser.getSelectedFile()
    } else {
      null
    }
  }
}

object NewAnimator extends swing.JFrame("Paintown Animator"){
  /* The paintown engine uses 90 ticks per second by default */
  var ticksPerSecond = 90
  var animator:NewAnimator = null
  def dataPath(f:File):File = {
    new File(Data.getDataPath().getPath() + "/" + f.getPath());
  }

  def setTicksPerSecond(ticks:Int){
    ticksPerSecond = ticks
  }

  def getTicksPerSecond() = ticksPerSecond

  def getNewFileChooser():RelativeFileChooser = 
    new RelativeFileChooser(animator, Data.getDataPath())

  def getNewFileChooser(path:File):RelativeFileChooser =
    new RelativeFileChooser(animator, Data.getDataPath(), path);

  def getFiles(path:String):List[String] = {
      val dir = dataPath(new File(path));
      var files = List[String]()
      /* use a FileFilter here */
      if (dir.isDirectory()){
          val all = dir.listFiles()
          for (file <- all){
              if (file.getName().endsWith(".png") ||
                  file.getName().endsWith(".tga") ||
                  file.getName().endsWith(".pcx") ||
                  file.getName().endsWith(".bmp")){
                    files = files :+ file.getName()
              }
          }
      }
      files.sort((path1, path2) => (path1 compareTo path2) < 0)
  }

}

object Animator2{
  def main(args: Array[String]):Unit = {
    val editor = new NewAnimator()
    System.out.println("Current working directory is " + System.getProperty("user.dir"))
    NewAnimator.animator = editor
    swing.SwingUtilities.invokeLater(new Runnable(){
      def run(){
        editor.setVisible(true);
        for (arg <- args){
          editor.loadPlayer(new File(arg))
        }
      }
    });
  }
}
