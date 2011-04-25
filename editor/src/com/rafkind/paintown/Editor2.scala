package com.rafkind.paintown;

import java.io._
import java.util.HashMap;
import javax.swing._
import java.awt._
import java.awt.image._
import java.awt.event._
import javax.swing.event._
import org.swixml.SwingEngine

// import java.util.List
import scala.collection.immutable.List

import com.rafkind.paintown.exception.LoadException
import com.rafkind.paintown.level.Level
import com.rafkind.paintown.level.Block
import com.rafkind.paintown.level.Thing
import com.rafkind.paintown.level.Character
import com.rafkind.paintown.level.Item
import javax.swing.filechooser.FileFilter

class NewEditor extends JFrame {
  construct();

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


    val tabbed = new JTabbedPane();
    this.getContentPane().add(tabbed);

    quit.addActionListener(new ActionListener(){
        override def actionPerformed(event:ActionEvent){
          Closer.close();
        }
      });

    animationEditor.addActionListener(new AbstractAction(){
        override def actionPerformed(event:ActionEvent){
          com.rafkind.paintown.animator.Animator.main(new Array[String](0));
        }
      });

    data.addActionListener(new ActionListener(){
        override def actionPerformed(event:ActionEvent){
          /* just a container for an object */
          class ObjectBox {
            var internal:Object = null;

            def set(o:Object){
              internal = o
            }

            def get(){
              internal
            }
          }

          val engine = new SwingEngine("data-path.xml");
          val path = engine.find("path").asInstanceOf[JTextField];
          val box = new ObjectBox();
          box.set(Data.getDataPath());
          path.setText(Data.getDataPath().getPath());
          val change = engine.find("change").asInstanceOf[JButton];
          change.addActionListener(new AbstractAction(){
              override def actionPerformed(event:ActionEvent){
                val chooser = new JFileChooser(new File("."));	
                chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
                val returnVal = chooser.showOpenDialog(NewEditor.this);
                if (returnVal == JFileChooser.APPROVE_OPTION){
                  val newPath = chooser.getSelectedFile();
                  path.setText(newPath.getPath());
                  box.set(newPath);
                }
              }
            });
          val save = engine.find("save").asInstanceOf[JButton];
          val cancel = engine.find("cancel").asInstanceOf[JButton];
          val dialog = new JDialog(NewEditor.this, "Paintown data path");
          save.addActionListener(new AbstractAction(){
              override def actionPerformed(event:ActionEvent){
                Data.setDataPath(box.get().asInstanceOf[File]);
                dialog.setVisible(false);
              }
            });
          cancel.addActionListener(new AbstractAction(){
              override def actionPerformed(event:ActionEvent){
                dialog.setVisible(false);
              }
            });
          val panel = engine.getRootComponent().asInstanceOf[JPanel];
          dialog.getContentPane().add(panel);
          dialog.setSize(300, 300);
          dialog.setVisible(true);
        }
      });


    val levels = new HashMap[Component, Level]();

    def doSave(level:Level, file:File){
        val out = new FileOutputStream(file);
        new PrintStream(out).print(level.toToken().toString() + "\n");
        out.close();
        System.out.println(level.toToken().toString());
      }

      newLevel.addActionListener(new AbstractAction(){
          override def actionPerformed(event:ActionEvent){
            val level = new Level();
            /* add 3 blocks to get the user started */
            for (i <- 1 to 3){
              level.getBlocks().asInstanceOf[java.util.List[Block]].add(new Block());
            }

            levels.put(tabbed.add(createEditPanel(level)), level);
          }
        });

        saveLevel.addActionListener(new AbstractAction(){
            override def actionPerformed(event:ActionEvent){
                if (tabbed.getSelectedComponent() != null){
                    val level = levels.get(tabbed.getSelectedComponent()).asInstanceOf[Level];
                    val file:File = level.getPath() match {
                      case null => userSelectFile("Save Level");
                      case what => what
                    }

                    /* write the text to a file */
                    if (file != null){
                        try{
                            doSave(level, file);
                            level.setPath(file);
                            tabbed.setTitleAt(tabbed.getSelectedIndex(), file.getName());
                        } catch {
                            case fail:Exception => {
                              fail.printStackTrace();
                              showError("Could not save " + file + " because " + fail.getMessage());
                            }
                        }
                    }
                }
            }
        });

      saveLevelAs.addActionListener(new AbstractAction(){
          override def actionPerformed(event:ActionEvent){
            if (tabbed.getSelectedComponent() != null){
              val level = levels.get(tabbed.getSelectedComponent()).asInstanceOf[Level];
              val file = userSelectFile("Save level as");
              /* write the text to a file */
              if (file != null){
                try{
                  doSave(level, file);
                  level.setPath(file);
                  tabbed.setTitleAt(tabbed.getSelectedIndex(), file.getName());
                } catch {
                  case fail:Exception => {
                    fail.printStackTrace();
                    showError("Could not save " + file + " because " + fail.getMessage() );
                  }
                }
              }
            }
          }
        });

      closeLevel.addActionListener( new AbstractAction(){
          override def actionPerformed(event:ActionEvent){
            if ( tabbed.getSelectedComponent() != null ){
              levels.remove( tabbed.getSelectedComponent() );
              tabbed.remove( tabbed.getSelectedComponent() );
            }
          }
        });

      loadLevel.addActionListener( new AbstractAction(){
          override def actionPerformed(event:ActionEvent){
            val chooser = new JFileChooser( new File( "." ) );	
            chooser.setFileFilter(new FileFilter(){
                def accept(f:File):Boolean = {
                  f.isDirectory() || f.getName().endsWith( ".txt" );
                }

                def getDescription():String = {
                  "Level files";
                }
              });

            val returnVal = chooser.showOpenDialog( NewEditor.this );
            if ( returnVal == JFileChooser.APPROVE_OPTION ){
              val f = chooser.getSelectedFile();
              try{
                val level = new Level(f);
                levels.put(tabbed.add(f.getName(), createEditPanel(level)), level);
              } catch {
                case fail:LoadException => {
                  showError( "Could not load " + f.getName() );
                  System.out.println( "Could not load " + f.getName() );
                  fail.printStackTrace();
                }
              }
            }
          }
        });

    this.setJMenuBar(menuBar);
    this.addWindowListener(new WindowAdapter{
      override def windowClosing(e:WindowEvent){
        Closer.close();
      }
    });
  }

  def createEditPanel(level:Level):JComponent = {
        val engine = new SwingEngine("main.xml");

        val viewContainer = engine.find( "view" ).asInstanceOf[JPanel];
        val viewScroll = new JScrollPane(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS, ScrollPaneConstants.HORIZONTAL_SCROLLBAR_ALWAYS);

        val view = new JPanel(){
            override def getPreferredSize():Dimension = {
                level.getSize()
            }

            override def paintComponent(g:Graphics){
                val h = viewScroll.getHorizontalScrollBar();
                val v = viewScroll.getVerticalScrollBar();
                g.setColor(new Color(64, 64, 64));
                g.fillRect(0, 0, level.getWidth().toInt, v.getVisibleAmount());
                g.clearRect(0, v.getVisibleAmount().toInt + 1, level.getWidth().toInt, level.getHeight().toInt);
                level.render(g.asInstanceOf[Graphics2D], h.getValue(), 0, h.getVisibleAmount(), v.getVisibleAmount());
            }
        };

        viewScroll.setPreferredSize(new Dimension(200, 200));
        viewScroll.setViewportView(view);

        /* this allows smooth scrolling of the level */
        viewScroll.getViewport().setScrollMode(JViewport.BACKINGSTORE_SCROLL_MODE);

        /*
           System.out.println( "JViewport.BLIT_SCROLL_MODE = " + JViewport.BLIT_SCROLL_MODE );
           System.out.println( "JViewport.BACKINGSTORE_SCROLL_MODE = " + JViewport.BACKINGSTORE_SCROLL_MODE );
           System.out.println( "JViewport.SIMPLE_SCROLL_MODE = " + JViewport.SIMPLE_SCROLL_MODE );
           System.out.println( "View scroll mode: " + viewScroll.getViewport().getScrollMode() );
           */
        viewScroll.getHorizontalScrollBar().setBackground(new Color(128, 255, 0));

        def editSelected(thing:Thing){
          val dialog = new JDialog(NewEditor.this, "Edit");
          dialog.setSize(350, 300);
          val editor = thing.getEditor();
          dialog.getContentPane().add(editor.createPane(level, new Lambda0(){
              override def invoke():Object = {
                dialog.setVisible(false);
                viewScroll.repaint();
                null
              }
            }));
          dialog.setVisible(true);
        }
  
        class ObjectListModel extends ListModel {
            var data:List[File] = defaultObjects();
            var listeners = List[ListDataListener]();

            def add(file:File){
                data = data :+ file
                val event = new ListDataEvent(this, ListDataEvent.INTERVAL_ADDED, data.size, data.size);
                for (listener <- listeners){
                    listener.intervalAdded(event)
                }
            }
            
            def remove(index:Int){
                data = data.remove(data.indexOf(_) == index)
                val event = new ListDataEvent(this, ListDataEvent.INTERVAL_REMOVED, index, index);
                for (listener <- listeners){
                    listener.intervalAdded(event);
                }
            }

            def getAll():List[File] = {
                data
            }

            override def addListDataListener(listener:ListDataListener){
                listeners = listeners :+ listener;
            }

            override def getElementAt(index:Int):Object = {
                this.data.find(data.indexOf(_) == index) match {
                    case Some(obj) => obj
                    case None => throw new Exception("failed to find " + index)
                }
            }

            override def getSize():Int = {
                this.data.size;
            }

            override def removeListDataListener(listener:ListDataListener){
                listeners = this.listeners - listener;
            }
        };

        val objectsModel = new ObjectListModel();

        class Mouser extends MouseMotionAdapter with MouseInputListener {
            var selected:Thing = null;
            var dx:Double = 0
            var dy:Double = 0
            var sx:Double = 0
            var sy:Double = 0
            var currentPopup:JDialog = null

            def getSelected():Thing = {
                selected;
            }

            def setSelected(thing:Thing){
                selected = thing;
            }

            override def mouseDragged(event:MouseEvent){
                if (selected != null){
                    // System.out.println( "sx,sy: " + sx + ", " + sy + " ex,ey: " + (event.getX() / 2) + ", " + (event.getY() / 2) + " dx, dy: " + dx + ", " + dy );
                    level.moveThing( selected, (sx + event.getX() / level.getScale() - dx).toInt, (sy + event.getY() / level.getScale() - dy).toInt);
                    viewScroll.repaint();
                }
            }

            def leftClick(event:MouseEvent):Boolean = {
                event.getButton() == MouseEvent.BUTTON1;
            }

            def rightClick(event:MouseEvent):Boolean = {
                event.getButton() == MouseEvent.BUTTON3;
            }

            def selectThing(event:MouseEvent){
                val thing = findThingAt(event)
                var has:Block = null;
                for (block <- scala.collection.JavaConversions.asScalaBuffer(level.getBlocks().asInstanceOf[java.util.List[Block]])){
                    block.setHighlight(false);
                    if (thing != null && block.hasThing(thing)){
                        has = block;
                    }
                }

                if (has != null){
                    has.setHighlight(true);
                    viewScroll.repaint();
                }

                if (selected == null && thing != null){
                    // selected = findThingAt( event );
                    selected = thing;
                    selected.setSelected(true);
                    sx = selected.getX();
                    sy = selected.getY() + level.getMinZ();
                    // System.out.println( "Y: " + selected.getY() + " minZ: " + level.getMinZ() );
                    dx = event.getX() / level.getScale();
                    dy = event.getY() / level.getScale();
                    // System.out.println( "Found: " + selected + " at " + event.getX() + " " + event.getY() );
                }

                if (getSelected() != null && event.getClickCount() == 2){
                    try{
                        editSelected(getSelected());
                    } catch {
                      case fail:Exception => {
                        fail.printStackTrace();
                      }
                    }
                }
            }

            def findFiles(dir:File, ending:String):List[File] = {
                val all = dir.listFiles(new java.io.FileFilter(){
                    override def accept(path:File):Boolean = {
                        path.isDirectory() || path.getName().endsWith(ending);
                    }
                });
                var files = List[File]()
                for (file <- all){
                    if (file.isDirectory() ){
                        files = files ++ findFiles(file, ending);
                    } else {
                        files = files :+ file
                    }
                }
                return files;
            }

            def findBlock(event:MouseEvent):Block = {
                val x = (event.getX() / level.getScale()).toInt
                // System.out.println( event.getX() + " -> " + x );
                return level.findBlock(x);
                /*
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
                   */
            }

            def makeThing(head:Token, x:Int, y:Int, path:String):Thing = {
                if (head.getName().equals("character")){
                    val temp = new Token();
                    temp.addToken(new Token("character"));
                    temp.addToken(("name" :: "TempName" :: List[String]()).toArray)
                    temp.addToken(("coords" :: x.toString :: y.toString :: List[String]()).toArray)
                    temp.addToken(("health" :: "40" :: List[String]()).toArray)
                    temp.addToken(("path" :: path :: List[String]()).toArray)
                    return new Character(temp);
                } else if (head.getName().equals("item")){
                    val temp = new Token();
                    temp.addToken(new Token("item"));
                    temp.addToken(("coords" :: x.toString :: y.toString :: List[String]()).toArray)
                    temp.addToken(("path" :: path :: List[String]()).toArray)
                    // System.out.println( "Make item from " + temp.toString() );
                    return new Item(temp);
                } else if (head.getName().equals("cat")){
                    val temp = new Token();
                    temp.addToken(new Token("item"));
                    temp.addToken(("coords" :: x.toString :: y.toString :: List[String]()).toArray)
                    temp.addToken(("path" :: path :: List[String]()).toArray)
                    return new Item(temp);
                }
                throw new LoadException("Unknown type: " + head.getName());
            }

            def collectCharFiles():List[File] = {
                return objectsModel.getAll()
            }

            def showAddObject(block:Block){
              val breaks = new scala.util.control.Breaks
              import breaks.{break, breakable}

              var x = -1;
              breakable{
                for (check <- scala.collection.JavaConversions.asScalaBuffer(level.getBlocks().asInstanceOf[java.util.List[Block]])){
                  if (check == block){
                    break;
                  }
                  if (check.isEnabled()){
                    x += check.getLength();
                  }
                }
              }
              showAddObjectPopup(new MouseEvent(NewEditor.this, -1, 0, 0, ((x + block.getLength() / 2) * level.getScale()).toInt, ((level.getMinZ() + level.getMaxZ()) * level.getScale() / 2).toInt, 1, false));
            }

            def showAddObjectPopup(event:MouseEvent){
                // JPanel panel = new JPanel();
                val files = collectCharFiles();
                val panel = Box.createVerticalBox();
                val all = new JList(new java.util.Vector(scala.collection.JavaConversions.asJavaList(files)));
                panel.add(new JScrollPane(all));
                val add = new JButton("Add");
                val close = new JButton("Close");
                val buttons = Box.createHorizontalBox();
                buttons.add(add);
                buttons.add(close);
                panel.add(buttons);
                if (currentPopup != null){
                    currentPopup.setVisible(false);
                }
                val dialog = new JDialog(NewEditor.this, "Add");
                dialog.getContentPane().add( panel );
                dialog.setSize(220, 250);
                dialog.setLocation(event.getX() - viewScroll.getHorizontalScrollBar().getValue(), event.getY());
                close.addActionListener(new AbstractAction(){
                    override def actionPerformed(event:ActionEvent){
                        dialog.setVisible(false);
                    }
                });
                currentPopup = dialog;
                dialog.setVisible(true);

                def addThing(file:File){
                    def mid(a:Int, b:Int, c:Int):Int = {
                        Math.max(Math.min(b, c), a);
                    }

                    try{
                      val block = findBlock(event);
                      if (block != null){
                        val reader = new TokenReader(dataPath(file));
                        val head = reader.nextToken();
                        var x = (event.getX() / level.getScale()).toInt;
                        val y = (event.getY() / level.getScale()).toInt;
                        val breaks = new scala.util.control.Breaks
                        import breaks.{break, breakable}
                        breakable{
                          for (check <- scala.collection.JavaConversions.asScalaBuffer(level.getBlocks().asInstanceOf[java.util.List[Block]])){
                            if (block == check){
                              break;
                            }
                            if (check.isEnabled()){
                              x -= check.getLength();
                            }
                          }
                        }
                        block.addThing(makeThing(head, x, mid(0, y - level.getMinZ(), level.getMaxZ() - level.getMinZ()), file.getPath()));
                        /*
                        Character c = new Character( reader.nextToken() );
                        b.add( new Character( reader.nextToken() ) );
                        */
                        viewScroll.repaint();
                      } else {
                        // JOptionPane.showMessageDialog( null, "The cursor is not within a block. Either move the cursor or add a block.", "Paintown Editor Error", JOptionPane.ERROR_MESSAGE );
                        showError("The cursor is not within a block. Either move the cursor or add a block.");
                      }
                    } catch {
                    case fail:LoadException => {
                      System.out.println("Could not load " + file);
                      fail.printStackTrace();
                    }
                  }
                }

                all.addMouseListener(new MouseAdapter(){
                    override def mouseClicked(clicked:MouseEvent){
                        if (clicked.getClickCount() == 2){
                            val index = all.locationToIndex(clicked.getPoint());
                            val file = get(files, index);
                            addThing(file);
                            dialog.setVisible(false);
                        }
                    }
                });

                add.addActionListener(new AbstractAction(){
                    override def actionPerformed(event:ActionEvent){
                        val index = all.getSelectedIndex();
                        if (index != -1){
                            val file = get(files, index);
                            addThing(file);
                            dialog.setVisible(false);
                        }
                    }
                });
            }

            def mousePressed(event:MouseEvent){
                if (leftClick(event)){
                    if (selected != null){
                        selected.setSelected(false);
                    }
                    selected = null;
                    selectThing(event);
                } else if (rightClick(event)){
                    showAddObjectPopup(event);
                }
            }

            def mouseExited(event:MouseEvent){
                if (selected != null){
                    // selected = null;
                    viewScroll.repaint();
                }
            }

            def findThingAt(event:MouseEvent):Thing = {
                level.findThing((event.getX() / level.getScale()).toInt,
                                (event.getY() / level.getScale()).toInt);
            }

            def mouseClicked(event:MouseEvent){
            }

            def mouseEntered(event:MouseEvent){
            }

            def mouseReleased(event:MouseEvent){
                if (selected != null ){
                    // selected = null;
                    viewScroll.repaint();
                }
            }
        }

        val mousey = new Mouser();

        view.addMouseMotionListener(mousey );
        view.addMouseListener(mousey );
        view.addMouseListener(new MouseAdapter(){
            override def mousePressed(event:MouseEvent){
                /* force focus to move to the view */
                view.requestFocusInWindow(); 
            }
        });

        val tabbed = engine.find("tabbed").asInstanceOf[JTabbedPane];
        val holder = Box.createVerticalBox();
        val blocks = Box.createVerticalBox();
        holder.add(new JScrollPane(blocks));
        holder.add(new JSeparator());

        class ObjectList extends ListModel {
          /* list listeners */
          var listeners:List[ListDataListener] = List[ListDataListener]()
          var things:List[Thing] = List[Thing]()
          var current:Block = null

          def update(thing:Thing){
            val count = 0;
            for (current <- things){
              if (current == thing){
                contentsChanged(new ListDataEvent(this, ListDataEvent.CONTENTS_CHANGED, count, count));
              }
            }
          }

          def setBlock(block:Block){
            current = block;
            if (block == null){
              this.things = List[Thing]()
            } else {
              this.things = toScalaList(block.getThings().asInstanceOf[java.util.List[Thing]]);
              for (thing <- things){
                thing.addListener(new Lambda1(){
                    override def invoke(o:Object):Object = {
                      update(o.asInstanceOf[Thing])
                      null
                    }
                })
              }
            }

            contentsChanged(new ListDataEvent(this, ListDataEvent.CONTENTS_CHANGED, 0, 999999));
          }

          def contentsChanged(event:ListDataEvent){
            for (listener <- listeners){
              listener.contentsChanged(event);
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

            def getBlock():Block = {
              current
            }

            override def addListDataListener(listener:ListDataListener){
                this.listeners = this.listeners :+ listener
            }

            override def getElementAt(index:Int):Object = {
                get(things, index)
            }

            override def getSize():Int = {
                this.things.size;
            }

            override def removeListDataListener(listener:ListDataListener){
                listeners = listeners - listener
            }
        }

        val objectList = new ObjectList();
        // final JList currentObjects = new JList( objectList );
        val blockObjectsEngine = new SwingEngine("block-objects.xml");
        // holder.add( new JLabel( "Objects" ) );
        // holder.add( new JScrollPane( currentObjects ) );
        val objectsAdd = blockObjectsEngine.find( "add" ).asInstanceOf[JButton];
        val objectsDelete = blockObjectsEngine.find( "delete" ).asInstanceOf[JButton];
        val objectsAddRandom = blockObjectsEngine.find( "add-random" ).asInstanceOf[JButton];
        val objectsDeleteAll = blockObjectsEngine.find( "delete-all" ).asInstanceOf[JButton];
        val currentObjects = blockObjectsEngine.find( "current" ).asInstanceOf[JList];
        currentObjects.setModel(objectList);
        holder.add(blockObjectsEngine.getRootComponent().asInstanceOf[JPanel]);

        holder.add(Box.createVerticalGlue());
        
        objectsDelete.addActionListener(new AbstractAction(){
            override def actionPerformed(event:ActionEvent){
                val block = objectList.getBlock();
                val thing = currentObjects.getSelectedValue().asInstanceOf[Thing];
                if (thing != null && block != null){
                    mousey.setSelected(null);
                    block.removeThing(thing);
                    objectList.setBlock(block);
                    viewScroll.repaint();
                }
            }
        });

        objectsDeleteAll.addActionListener(new AbstractAction(){
            override def actionPerformed(event:ActionEvent){
                val block = objectList.getBlock();
                if (block != null){
                    mousey.setSelected( null );
                    block.removeAllThings();
                    objectList.setBlock(block);
                    viewScroll.repaint();
                }
            }
        });

        objectsAdd.addActionListener(new AbstractAction(){
            override def actionPerformed(event:ActionEvent){
                try{
                    if (objectList.getBlock() == null){
                        throw new EditorException("Select a block");
                    }
                    mousey.showAddObject(objectList.getBlock());
                } catch {
                  case fail:EditorException => {
                    showError(fail);
                  }
                }
            }
        });

        objectsAddRandom.addActionListener(new AbstractAction(){
            private val thingsToAdd = 4;
            private val makeName = new RandomNameAction("boys.txt"){
                override def actionPerformed(event:ActionEvent){
                }
            };

            private def randomX():Int = {
                if (Math.random > 0.5){
                    -((Math.random * 640) + 320).toInt;
                } else {
                    (Math.random * 800).toInt + 350;
                }
            }

            private def randomY():Int = {
                (Math.random * (level.getMaxZ() - level.getMinZ())).toInt;
            }

            private def randomHealth():Int = {
                (Math.random * 60).toInt + 20;
            }

            private def make():Character = {
                val choose = objectsModel.getElementAt((Math.random * (objectsModel.getSize())).toInt).asInstanceOf[File];
                val temp = new Token();
                temp.addToken(new Token("character"));
                temp.addToken(("name" :: "TempName" :: List[String]()).toArray)
                temp.addToken(("coords" :: randomX().toString :: randomY().toString :: List[String]()).toArray)
                temp.addToken(("health" :: randomHealth().toString :: List[String]()).toArray)
                temp.addToken(("path" :: choose.getPath() :: List[String]()).toArray)
                val guy = new Character(temp);
                guy.setMap((Math.random * guy.getMaxMaps()).toInt);
                return guy;
            }

            override def actionPerformed(event:ActionEvent){
              try{
                if (objectList.getBlock() == null){
                  throw new EditorException("Select a block");
                }
                for (index <- 1 to thingsToAdd){
                  try{
                    objectList.getBlock().addThing(make());
                  } catch {
                    case fail:LoadException => {
                      System.out.println("Ignoring exception");
                      fail.printStackTrace();
                    }
                  }
                }
                objectList.setBlock(objectList.getBlock());
                viewScroll.repaint();
              } catch {
                case fail:EditorException => {
                  showError(fail);
                }
              }
            }
        });

        /*

        / * if an object is selected highlight it and scroll over to it * /
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

                / * the current X position within the world * /
                int currentX = 0;
                Block b = level.findBlock( t );
                b.setHighlight( true );

                / * calculate absolute X position of the selected thing * /
                for ( Iterator it = level.getBlocks().iterator(); it.hasNext(); ){
                    Block next = (Block) it.next();
                    if ( next == b ){
                        break;
                    }
                    if ( next.isEnabled() ){
                        currentX += next.getLength();
                    }
                }

                currentX += t.getX();
                / * show the object in the center of the view * /
                int move = (int)(currentX * level.getScale() - viewScroll.getHorizontalScrollBar().getVisibleAmount() / 2);

                / * scroll over to the selected thing * /
                // viewScroll.getHorizontalScrollBar().setValue( move );
                smoothScroll( viewScroll.getHorizontalScrollBar(), viewScroll.getHorizontalScrollBar().getValue(), move );

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

        / * so the user can click on the scrolly pane * /
        // viewScroll.setFocusable( true );
        view.setFocusable( true );

        view.getInputMap().put( KeyStroke.getKeyStroke( KeyEvent.VK_DELETE, 0 ), "delete" );

        / * ctrl-c * /
        view.getInputMap().put( KeyStroke.getKeyStroke( KeyEvent.VK_C, 2 ), "copy" );
        / * ctrl-v * /
        view.getInputMap().put( KeyStroke.getKeyStroke( KeyEvent.VK_V, 2 ), "paste" );
        / * ctrl-b * /
        view.getInputMap().put( KeyStroke.getKeyStroke( KeyEvent.VK_B, 2 ), "change-boy-name" );
        view.getInputMap().put( KeyStroke.getKeyStroke( KeyEvent.VK_G, 2 ), "change-girl-name" );

        currentObjects.getInputMap().put( KeyStroke.getKeyStroke( KeyEvent.VK_B, 2 ), "change-boy-name" );
        currentObjects.getInputMap().put( KeyStroke.getKeyStroke( KeyEvent.VK_G, 2 ), "change-girl-name" );

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

        AbstractAction changeBoy = new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                if ( mousey.getSelected() != null && mousey.getSelected() instanceof Character ){
                    Character guy = (Character) mousey.getSelected();
                    guy.setName( Editor.this.generateBoysName() );
                }
            }
        };

        AbstractAction changeGirl = new AbstractAction(){
            public void actionPerformed( ActionEvent event ){
                if ( mousey.getSelected() != null && mousey.getSelected() instanceof Character ){
                    Character guy = (Character) mousey.getSelected();
                    guy.setName( Editor.this.generateGirlsName() );
                }
            }
        };

        view.getActionMap().put( "change-boy-name", changeBoy );
        view.getActionMap().put( "change-girl-name", changeGirl );
        currentObjects.getActionMap().put( "change-boy-name", changeBoy );
        currentObjects.getActionMap().put( "change-girl-name", changeGirl );

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
                / * middle of the current screen * /
                int x = (int)(viewScroll.getHorizontalScrollBar().getValue() / level.getScale() + viewScroll.getHorizontalScrollBar().getVisibleAmount() / level.getScale() / 2 );
                / * in between the min and max z lines * /
                int y = (int)((level.getMaxZ() - level.getMinZ()) / 2);
                Block b = level.findBlock( x );
                if ( b != null && getCopy() != null ){
                    Thing copy = getCopy().copy();
                    / * x has to be relative to the beginning of the block * /
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
        / * objectsModel is declared way up top * /
        objects.setModel( objectsModel  );

        {
            final JButton add = (JButton) objectEngine.find( "add" );
            final JButton remove = (JButton) objectEngine.find( "delete" );

            add.addActionListener( new AbstractAction(){
                public void actionPerformed( ActionEvent event ){
                    RelativeFileChooser chooser = new RelativeFileChooser( Editor.this, Data.getDataPath() );
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
        // debugSwixml( levelEngine );
        final JPanel levelPane = (JPanel) levelEngine.getRootComponent();
        tabbed.add( "Level", levelPane );

        final JSpinner levelMinZ = (JSpinner) levelEngine.find( "min-z" );
        final JSpinner levelMaxZ = (JSpinner) levelEngine.find( "max-z" );
        final JComboBox atmosphere = (JComboBox) levelEngine.find( "atmosphere" );
        final JTextField levelBackground = (JTextField) levelEngine.find( "background" );
        final JTextField levelDescription = (JTextField) levelEngine.find("description");
        final JButton levelChangeBackground = (JButton) levelEngine.find( "change-background" );
        final Vector frontPanelsData = new Vector();
        final JList frontPanels = (JList) levelEngine.find( "front-panels" );
        frontPanels.setListData( frontPanelsData );
        final Vector backPanelsData = new Vector();
        final JList backPanels = (JList) levelEngine.find( "back-panels" );
        final JTextArea order = (JTextArea) levelEngine.find( "order" );
        final JComboBox pickOrder = (JComboBox) levelEngine.find( "pick-order" );
        final JSlider backgroundParallax = (JSlider) levelEngine.find( "background-parallax-slider" );
        final JLabel backgroundAmount = (JLabel) levelEngine.find( "background-parallax-amount" );
        final JSlider foregroundParallax = (JSlider) levelEngine.find( "foreground-parallax-slider" );
        final JLabel foregroundAmount = (JLabel) levelEngine.find( "foreground-parallax-amount" );

        foregroundAmount.setText( String.valueOf( level.getForegroundParallax() ) );
        backgroundAmount.setText( String.valueOf( level.getBackgroundParallax() ) );
        backgroundParallax.setValue( (int) level.getBackgroundParallax() );
        foregroundParallax.setValue( (int) (level.getForegroundParallax() * 10) );

        backgroundParallax.addChangeListener( new ChangeListener(){
            public void stateChanged( ChangeEvent e ){
                level.setBackgroundParallax( (double) backgroundParallax.getValue() );
                backgroundAmount.setText( String.valueOf( level.getBackgroundParallax() ) );
            }
        });

        foregroundParallax.addChangeListener( new ChangeListener(){
            public void stateChanged( ChangeEvent e ){
                level.setForegroundParallax( (double) foregroundParallax.getValue() / 10.0 );
                foregroundAmount.setText( String.valueOf( level.getForegroundParallax() ) );
            }
        });
        
        levelDescription.setText(level.getDescription());
        levelDescription.addActionListener(new AbstractAction(){
            public void actionPerformed(ActionEvent e){
                level.setDescription(levelDescription.getText());
            }
        });

        atmosphere.addItem( null );
        atmosphere.addItem( "rain" );
        atmosphere.addItem( "snow" );
        atmosphere.addItem( "night" );
        atmosphere.addItem( "fog" );

        atmosphere.setSelectedItem( level.getAtmosphere() );

        atmosphere.addActionListener( new ActionListener(){
            public void actionPerformed( ActionEvent e ){
                level.setAtmosphere( (String) atmosphere.getSelectedItem() );
            }
        });

        atmosphere.setEditable( false );

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

        { / * force scope * /
            final JButton add = (JButton) levelEngine.find( "add-front-panel" );
            add.addActionListener( new AbstractAction(){
                public void actionPerformed( ActionEvent event ){
                    RelativeFileChooser chooser = new RelativeFileChooser( Editor.this, Data.getDataPath() );
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

        { / * force scope * /
            final JButton add = (JButton) levelEngine.find( "add-back-panel" );
            add.addActionListener( new AbstractAction(){
                public void actionPerformed( ActionEvent event ){
                    RelativeFileChooser chooser = new RelativeFileChooser( Editor.this, Data.getDataPath() );
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
                RelativeFileChooser chooser = new RelativeFileChooser( Editor.this, Data.getDataPath() );
                int ret = chooser.open();
                if ( ret == RelativeFileChooser.OK ){
                    final String path = chooser.getPath();
                    level.loadBackground( path );
                    levelBackground.setText( path );
                    viewScroll.repaint();
                }
            }
        });

        / * initialize all the other crap for a level * /
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

        / * mess around with layout nonsense * /
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
                final JCheckBox isFinish = (JCheckBox) engine.find( "is-finish" );
                final JSpinner id = (JSpinner) engine.find("id");
                final JCheckBox isContinuous = (JCheckBox) engine.find( "is-continuous" );
                final JButton save = (JButton) engine.find( "save" );
                final JButton close = (JButton) engine.find( "close" );

                length.setText( String.valueOf( block.getLength() ) );
                isContinuous.setSelected( block.isContinuous() );
                id.setValue(block.getId());
                isFinish.setSelected( block.isFinish() );
                finish.setEnabled( block.isFinish() );
                finish.setText( String.valueOf( block.getFinish() ) );
                isFinish.addActionListener( new AbstractAction(){
                    public void actionPerformed( ActionEvent event ){
                        finish.setEnabled( isFinish.isSelected() );
                    }
                });

                save.addActionListener( new AbstractAction(){
                    public void actionPerformed( ActionEvent event ){
                        block.setLength( Integer.parseInt( length.getText() ) );
                        block.setId(((Integer) id.getValue()).intValue());
                        if ( isFinish.isSelected() ){
                            block.setFinish( Integer.parseInt( finish.getText() ) );
                        } else {
                            block.setFinish( 0 );
                        }
                        block.setContinuous( isContinuous.isSelected() );
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

            private void scrollToBlock( Block block ){
                int length = 0;
                for ( Iterator it = level.getBlocks().iterator(); it.hasNext(); ){
                    Block b = (Block) it.next();
                    if ( b == block ){
                        break;
                    }
                    if ( b.isEnabled() ){
                        length += b.getLength();
                    }
                }
                smoothScroll( viewScroll.getHorizontalScrollBar(), viewScroll.getHorizontalScrollBar().getValue(), (int)(length * level.getScale() - 15) );
                // viewScroll.getHorizontalScrollBar().setValue( (int)(length * level.getScale() - 10) );
            }

            / * self_ should be the 'setupBlocks' lambda so that it can
             * call itself recursively
             * /
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

                    check.setToolTipText("Check this box to make the block appear in the game");

                    check.setSelected( true );
                    stuff.add( check );
                    final JButton button = new JButton( "Block " + n + " : " + block.getLength() );
                    button.addActionListener( new AbstractAction(){
                        public void actionPerformed( ActionEvent event ){
                            if ( block.isEnabled() ){
                                scrollToBlock( block );
                                objectList.setBlock( block );
                            }
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

        final JSlider scroll = (JSlider) engine.find( "level-scale" );
        final JLabel scale = (JLabel) engine.find( "scale" );
        scroll.addChangeListener( new ChangeListener(){
            public void stateChanged( ChangeEvent e ){
                level.setScale( (double) scroll.getValue() * 2.0 / scroll.getMaximum() );
                scale.setText( "Scale: " + level.getScale() );
                view.revalidate();
                viewScroll.repaint();
            }
        });
        */

        engine.getRootComponent().asInstanceOf[JSplitPane];
    }

    def defaultObjects():List[File] = {
        List[File]() :+ 
        new File("chars/angel/angel.txt") :+
        new File("chars/billy/billy.txt") :+
        new File("chars/eiji/eiji.txt") :+
        new File("chars/heavy/heavy.txt") :+
        new File("chars/jhun/jhun.txt") :+
        new File("chars/joe/joe.txt") :+
        new File("chars/punk/punk.txt") :+
        new File("chars/ralf/ralf.txt") :+
        new File("chars/robert/robert.txt") :+
        new File("chars/rugal/rugal.txt") :+
        new File("chars/shermie/shermie.txt") :+
        new File("chars/yamazaki/yamazaki.txt") :+
        new File("chars/yashiro/yashiro.txt") :+
        new File("misc/apple/apple.txt") :+
        new File("misc/cake/cake.txt") :+
        new File("misc/chicken/chicken.txt") :+
        new File("misc/cat/cat.txt")
    }

    def dataPath(file:File):File = {
      new File(Data.getDataPath().getPath() + "/" + file.getPath());
    }

  def userSelectFile(title:String):File = {
    val chooser = new JFileChooser(new File("."));
    chooser.setDialogTitle(title);
    chooser.showOpenDialog(NewEditor.this) match {
      case JFileChooser.APPROVE_OPTION => chooser.getSelectedFile()
      case _ => null
    }
  }
    
  def showError(message:String){
    JOptionPane.showMessageDialog(null, message, "Paintown Editor Error", JOptionPane.ERROR_MESSAGE);
  }

  def showError(message:EditorException){
    showError(message.getMessage());
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
