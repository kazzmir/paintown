package com.rafkind.paintown.level

import java.io._
import java.util.HashMap
import javax.swing._
import java.awt._
import java.awt.image._
import java.awt.event._
import javax.swing.event._
import org.swixml.SwingEngine

// import java.util.List
import scala.collection.immutable.List

import com.rafkind.paintown.exception.LoadException
import com.rafkind.paintown.level.objects.Level
import com.rafkind.paintown.level.objects.Block
import com.rafkind.paintown.level.objects.Thing
import com.rafkind.paintown.level.objects.Character
import com.rafkind.paintown.level.objects.Item
import com.rafkind.paintown.Closer
import com.rafkind.paintown.Data
import com.rafkind.paintown.Lambda0
import com.rafkind.paintown.Lambda1
import com.rafkind.paintown.Lambda2
import com.rafkind.paintown.Token
import com.rafkind.paintown.TokenReader
import com.rafkind.paintown.RelativeFileChooser
import javax.swing.filechooser.FileFilter

class NewEditor extends JFrame("Paintown Editor"){
  var copy:Thing = null;

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
    this.setSize((screen.getWidth() * 0.9).toInt,
                 (screen.getHeight() * 0.9).toInt);

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
          com.rafkind.paintown.animator.Animator2.main(new Array[String](0));
        }
      });

    data.addActionListener(new ActionListener(){
        override def actionPerformed(event:ActionEvent){
          /* just a container for an object */
          class ObjectBox {
            var _internal:Object = null

            def internal = _internal 
            def internal_= (obj:Object):Unit = _internal = obj
          }

          val engine = new SwingEngine("data-path.xml");
          val path = engine.find("path").asInstanceOf[JTextField];
          val box = new ObjectBox();
          box.internal = Data.getDataPath()
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
                  box.internal = newPath
                }
              }
            });
          val save = engine.find("save").asInstanceOf[JButton];
          val cancel = engine.find("cancel").asInstanceOf[JButton];
          val dialog = new JDialog(NewEditor.this, "Paintown data path");
          save.addActionListener(new AbstractAction(){
              override def actionPerformed(event:ActionEvent){
                Data.setDataPath(box.internal.asInstanceOf[File]);
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
            SwingUtilities.invokeLater(new Runnable(){
                def run(){
                  val level = new Level();
                  /* add 3 blocks to get the user started */
                  for (i <- 1 to 3){
                    level.getBlocks().asInstanceOf[java.util.List[Block]].add(new Block());
                  }

                  levels.put(tabbed.add(createEditPanel(level)), level);
                }
              });
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
          dialog.setSize(350, 350);
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
  
        class ObjectListModel extends ListModel[File] {
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

            override def getElementAt(index:Int) = {
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

        class ObjectList extends ListModel[Thing] {
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

            override def getElementAt(index:Int) = {
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
        val currentObjects = blockObjectsEngine.find( "current" ).asInstanceOf[JList[Thing]];
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
                val choose = objectsModel.getElementAt((Math.random * (objectsModel.getSize())).toInt)
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

        /* if an object is selected highlight it and scroll over to it */
        currentObjects.addListSelectionListener(new ListSelectionListener(){
            def valueChanged(event:ListSelectionEvent){
                val thing = currentObjects.getSelectedValue()
                if (mousey.getSelected() != null){
                    val old = mousey.getSelected();
                    old.setSelected(false);
                    level.findBlock(old).setHighlight(false);
                }
                thing.setSelected(true);
                mousey.setSelected(thing);

                /* the current X position within the world */
                var currentX = 0;
                val block = level.findBlock(thing);
                block.setHighlight(true);
              
                val breaks = new scala.util.control.Breaks
                import breaks.{break, breakable}

                /* calculate absolute X position of the selected thing */
                breakable{
                  for (next <- scala.collection.JavaConversions.asScalaBuffer(level.getBlocks().asInstanceOf[java.util.List[Block]])){
                    if (next == block){
                      break;
                    }
                    if (next.isEnabled()){
                      currentX += next.getLength();
                    }
                  }
                }

                currentX += thing.getX();
                /* show the object in the center of the view */
                val move = (currentX * level.getScale() - viewScroll.getHorizontalScrollBar().getVisibleAmount() / 2).toInt;

                /* scroll over to the selected thing */
                // viewScroll.getHorizontalScrollBar().setValue( move );
                smoothScroll(viewScroll.getHorizontalScrollBar(), viewScroll.getHorizontalScrollBar().getValue(), move);

                viewScroll.repaint();
            }
        });

        currentObjects.getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_DELETE, 0), "delete");
        currentObjects.getActionMap().put("delete", new AbstractAction(){
            override def actionPerformed(event:ActionEvent){
              val thing = currentObjects.getSelectedValue()
              if (thing != null){
                mousey.setSelected(null);
                val block = level.findBlock(thing);
                block.removeThing(thing);
                objectList.setBlock(block);
                viewScroll.repaint();
              }
            }
          });

        currentObjects.addMouseListener(new MouseAdapter() {
            override def mouseClicked(clicked:MouseEvent){
              if (clicked.getClickCount() == 2){
                val thing = currentObjects.getSelectedValue()
                editSelected(thing);
              }
            }
          });

        /* so the user can click on the scrolly pane */
        // viewScroll.setFocusable( true );
        view.setFocusable(true);

        view.getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_DELETE, 0), "delete");

        /* ctrl-c */
        view.getInputMap().put( KeyStroke.getKeyStroke( KeyEvent.VK_C, 2 ), "copy" );
        /* ctrl-v */
        view.getInputMap().put( KeyStroke.getKeyStroke( KeyEvent.VK_V, 2 ), "paste" );
        /* ctrl-b */
        view.getInputMap().put( KeyStroke.getKeyStroke( KeyEvent.VK_B, 2 ), "change-boy-name" );
        view.getInputMap().put( KeyStroke.getKeyStroke( KeyEvent.VK_G, 2 ), "change-girl-name" );

        currentObjects.getInputMap().put( KeyStroke.getKeyStroke( KeyEvent.VK_B, 2 ), "change-boy-name" );
        currentObjects.getInputMap().put( KeyStroke.getKeyStroke( KeyEvent.VK_G, 2 ), "change-girl-name" );

        view.getActionMap().put( "delete", new AbstractAction(){
            override def actionPerformed(event:ActionEvent){
              if (mousey.getSelected() != null){
                val block = level.findBlock(mousey.getSelected());
                block.removeThing(mousey.getSelected());
                mousey.setSelected(null);
                objectList.setBlock(block);
                viewScroll.repaint();
              }
            }
          });

        view.getActionMap().put( "copy", new AbstractAction(){
            override def actionPerformed(event:ActionEvent){
                // System.out.println( "Copy object" );
                if (mousey.getSelected() != null){
                    setCopy(mousey.getSelected().copy());
                }
            }
        });

        val changeBoy = new AbstractAction(){
            override def actionPerformed(event:ActionEvent){
                if (mousey.getSelected() != null && mousey.getSelected().isInstanceOf[Character]){
                    val guy = mousey.getSelected().asInstanceOf[Character];
                    guy.setName(NewEditor.this.generateBoysName());
                }
            }
        };

        val changeGirl = new AbstractAction(){
            override def actionPerformed(event:ActionEvent){
                if ( mousey.getSelected() != null && mousey.getSelected().isInstanceOf[Character]){
                    val guy = mousey.getSelected().asInstanceOf[Character];
                    guy.setName(NewEditor.this.generateGirlsName() );
                }
            }
        };

        view.getActionMap().put( "change-boy-name", changeBoy );
        view.getActionMap().put( "change-girl-name", changeGirl );
        currentObjects.getActionMap().put( "change-boy-name", changeBoy );
        currentObjects.getActionMap().put( "change-girl-name", changeGirl );

        view.getActionMap().put( "paste", new AbstractAction(){
            def calculateLength(blocks:List[Block]):Int = {
                var total = 0
                for (block <- blocks){
                    total += block.getLength();
                }
                return total;
            }

            override def actionPerformed(event:ActionEvent){
                /* middle of the current screen */
                val x = (viewScroll.getHorizontalScrollBar().getValue() / level.getScale() + viewScroll.getHorizontalScrollBar().getVisibleAmount() / level.getScale() / 2 ).toInt;
                /* in between the min and max z lines */
                val y = ((level.getMaxZ() - level.getMinZ()) / 2).toInt;
                val block = level.findBlock(x);
                if (block != null && getCopy() != null){
                    val copy = getCopy().copy();
                    /* x has to be relative to the beginning of the block */
                    copy.setX(x - calculateLength(toScalaList(level.getBlocks().subList(0, level.getBlocks().indexOf(block)).asInstanceOf[java.util.List[Block]])));
                    copy.setY(y);
                    block.addThing(copy);
                    objectList.setBlock(block);
                    viewScroll.repaint();
                } else {
                    println("No block found at " + x);
                }
            }
        });

        tabbed.add("Blocks", holder);

        val objectEngine = new SwingEngine("objects.xml");
        tabbed.add("Objects", objectEngine.getRootComponent().asInstanceOf[JComponent]);

        // final JList objects = new JList( allowableObjects );
        val objects = objectEngine.find( "objects" ).asInstanceOf[JList[File]];
        /* objectsModel is declared way up top */
        objects.setModel(objectsModel);

        {
            val add = objectEngine.find( "add" ).asInstanceOf[JButton];
            val remove = objectEngine.find( "delete" ).asInstanceOf[JButton];

            add.addActionListener(new AbstractAction(){
                override def actionPerformed(event:ActionEvent){
                    val chooser = new RelativeFileChooser(NewEditor.this, "Choose a file", Data.getDataPath());
                    val ret = chooser.open();
                    if (ret == RelativeFileChooser.OK ){
                        val path = chooser.getPath();
                        objectsModel.add(new File(path));
                    }
                }
            });

            remove.addActionListener( new AbstractAction(){
                override def actionPerformed(event:ActionEvent){
                    val index = objects.getSelectedIndex();
                    if (index != -1){
                        objectsModel.remove(index);
                    }
                }
            });
        }

        val levelEngine = new SwingEngine( "level.xml" );
        // debugSwixml( levelEngine );
        val levelPane = levelEngine.getRootComponent().asInstanceOf[JPanel];
        tabbed.add("Level", levelPane);

        val levelMinZ = levelEngine.find( "min-z" ).asInstanceOf[JSpinner];
        val levelMaxZ = levelEngine.find( "max-z" ).asInstanceOf[JSpinner];
        val atmosphere = levelEngine.find( "atmosphere" ).asInstanceOf[JComboBox[String]];
        val levelBackground = levelEngine.find( "background" ).asInstanceOf[JTextField];
        val levelDescription = levelEngine.find("description").asInstanceOf[JTextField];
        val levelChangeBackground = levelEngine.find( "change-background" ).asInstanceOf[JButton];
        val frontPanelsData = new java.util.Vector[String]();
        val frontPanels = levelEngine.find( "front-panels" ).asInstanceOf[JList[String]];
        frontPanels.setListData(frontPanelsData);
        val backPanelsData = new java.util.Vector[String]();
        val backPanels = levelEngine.find( "back-panels" ).asInstanceOf[JList[String]];
        val order = levelEngine.find( "order" ).asInstanceOf[JTextArea];
        val pickOrder = levelEngine.find( "pick-order" ).asInstanceOf[JComboBox[String]];
        val backgroundParallax = levelEngine.find( "background-parallax-slider" ).asInstanceOf[JSlider];
        val backgroundAmount = levelEngine.find( "background-parallax-amount" ).asInstanceOf[JLabel];
        val foregroundParallax = levelEngine.find( "foreground-parallax-slider" ).asInstanceOf[JSlider];
        val foregroundAmount = levelEngine.find( "foreground-parallax-amount" ).asInstanceOf[JLabel];

        foregroundAmount.setText(level.getForegroundParallax().toString);
        backgroundAmount.setText(level.getBackgroundParallax().toString);
        backgroundParallax.setValue(level.getBackgroundParallax().toInt);
        foregroundParallax.setValue((level.getForegroundParallax() * 10).toInt);

        backgroundParallax.addChangeListener( new ChangeListener(){
            override def stateChanged(event:ChangeEvent){
                level.setBackgroundParallax(backgroundParallax.getValue().toDouble);
                backgroundAmount.setText(level.getBackgroundParallax().toString);
            }
        });

        foregroundParallax.addChangeListener( new ChangeListener(){
            override def stateChanged(event:ChangeEvent){
                level.setForegroundParallax(foregroundParallax.getValue().toDouble / 10.0);
                foregroundAmount.setText(level.getForegroundParallax().toString);
            }
        });
        
        levelDescription.setText(level.getDescription());
        levelDescription.addActionListener(new AbstractAction(){
            override def actionPerformed(event:ActionEvent){
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
            override def actionPerformed(event:ActionEvent){
                level.setAtmosphere(atmosphere.getSelectedItem().asInstanceOf[String]);
            }
        });

        atmosphere.setEditable( false );

        /* FIXME: the type parameter should probably be something other than Object */
        class BackPanelCombo(data:java.util.Vector[String]) extends ComboBoxModel[String] {
            var selected:String = null
            var listeners:List[ListDataListener] = List[ListDataListener]()

            def getSelectedItem() = {
                selected
            }

            def update(){
                selected = null
                val event = new ListDataEvent(this, ListDataEvent.CONTENTS_CHANGED, 0, 99999)
                for (listener <- listeners){
                    listener.contentsChanged(event);
                }
            }

            /* FIXME: what is going on here? why does scala force us to give
             * a type parameter for ComboBoxModel and then the setSelectedItem
             * method takes an Object? Looks like someone screwed up in Java-land..
             */
            def setSelectedItem(item:Object){
                selected = item.asInstanceOf[String]
            }

            override def addListDataListener(listener:ListDataListener){
                listeners = listeners :+ listener
            }

            override def getElementAt(index:Int) = {
                this.data.get(index)
            }

            override def getSize():Int = {
                this.data.size()
            }

            override def removeListDataListener(listener:ListDataListener){
                this.listeners = this.listeners - listener
            }
        }

        val comboModel = new BackPanelCombo(backPanelsData);
        pickOrder.setModel(comboModel);

        def setOrderText(){
          val orderText = new StringBuffer();
          for (order <- toScalaList[java.lang.Integer](level.getBackPanelOrder().asInstanceOf[java.util.List[java.lang.Integer]])){
            val name = level.getBackPanelName(order.intValue())
            orderText.append(name).append("\n");
          }
          order.setText(orderText.toString());
        };

        {
            val add = levelEngine.find("add-order").asInstanceOf[JButton];
            val remove = levelEngine.find( "remove-order" ).asInstanceOf[JButton];
            add.addActionListener( new AbstractAction(){
                override def actionPerformed(event:ActionEvent){
                    val path = pickOrder.getSelectedItem().asInstanceOf[String];
                    if (path != null){
                        level.addBackPanelOrder(path);
                        setOrderText();
                        viewScroll.repaint();
                    }
                }
            });

            remove.addActionListener( new AbstractAction(){
                override def actionPerformed(event:ActionEvent){
                    level.removeLastOrder();
                    setOrderText()
                    viewScroll.repaint();
                }
            });
        }

        { /* force scope */
            val add = levelEngine.find( "add-front-panel" ).asInstanceOf[JButton];
            add.addActionListener( new AbstractAction(){
                override def actionPerformed(event:ActionEvent){
                    val chooser = new RelativeFileChooser(NewEditor.this, "Choose a file", Data.getDataPath());
                    val ret = chooser.open();
                    if (ret == RelativeFileChooser.OK){
                        try{
                            val path:String = chooser.getPath();
                            level.addFrontPanel(path);
                            frontPanelsData.add(path);
                            frontPanels.setListData(frontPanelsData);
                            viewScroll.repaint();
                        } catch {
                          case fail:LoadException => {
                            fail.printStackTrace();
                          }
                        }
                    }
                }
            });

            val remove = levelEngine.find( "delete-front-panel" ).asInstanceOf[JButton];
            remove.addActionListener( new AbstractAction(){
                override def actionPerformed(event:ActionEvent){
                    if (frontPanels.getSelectedValue() != null){
                        val path = frontPanels.getSelectedValue().asInstanceOf[String];
                        level.removeFrontPanel(path);
                        frontPanelsData.remove(path);
                        frontPanels.setListData(frontPanelsData);
                        viewScroll.repaint();
                    }
                }
            });
        }

        { /* force scope */
            val add = levelEngine.find( "add-back-panel" ).asInstanceOf[JButton];
            add.addActionListener( new AbstractAction(){
                override def actionPerformed(event:ActionEvent){
                    val chooser = new RelativeFileChooser(NewEditor.this, "Choose a file", Data.getDataPath());
                    val ret = chooser.open();
                    if (ret == RelativeFileChooser.OK){
                        try{
                            val path = chooser.getPath();
                            level.addBackPanel(path);
                            backPanelsData.add(path);
                            backPanels.setListData(backPanelsData);
                            comboModel.update();
                            viewScroll.repaint();
                        } catch {
                          case fail:LoadException => {
                            fail.printStackTrace();
                          }
                        }
                    }
                }
            });

            val remove = levelEngine.find( "delete-back-panel" ).asInstanceOf[JButton];
            remove.addActionListener( new AbstractAction(){
                override def actionPerformed(event:ActionEvent){
                    if (backPanels.getSelectedValue() != null){
                        val path = backPanels.getSelectedValue().asInstanceOf[String];
                        level.removeBackPanel(path);
                        backPanelsData.remove(path);
                        backPanels.setListData(backPanelsData);
                        setOrderText()
                        comboModel.update();
                        viewScroll.repaint();
                    }
                }
            });
        }

        levelMinZ.setModel(new SpinnerNumberModel());
        levelMinZ.addChangeListener(new ChangeListener(){
            override def stateChanged(event:ChangeEvent){
                val spinner = event.getSource().asInstanceOf[JSpinner];
                val i = spinner.getValue().asInstanceOf[java.lang.Integer];
                level.setMinZ(i.intValue());
                viewScroll.repaint();
            }
        });

        levelMaxZ.setModel( new SpinnerNumberModel() );
        levelMaxZ.addChangeListener( new ChangeListener(){
            override def stateChanged(event:ChangeEvent){
                val spinner = event.getSource().asInstanceOf[JSpinner];
                val i = spinner.getValue().asInstanceOf[java.lang.Integer];
                level.setMaxZ(i.intValue());
                viewScroll.repaint();
            }
        });

        levelBackground.addActionListener(new AbstractAction(){
            override def actionPerformed(event:ActionEvent){
                level.loadBackground(levelBackground.getText());
                viewScroll.repaint();
            }
        });

        levelChangeBackground.addActionListener(new AbstractAction(){
            override def actionPerformed(event:ActionEvent){
                val chooser = new RelativeFileChooser(NewEditor.this, "Choose a file", Data.getDataPath());
                val ret = chooser.open();
                if (ret == RelativeFileChooser.OK){
                    val path = chooser.getPath();
                    level.loadBackground(path);
                    levelBackground.setText(path);
                    viewScroll.repaint();
                }
            }
        });

        val introText = levelEngine.find("intro").asInstanceOf[JTextField]
        introText.setText(level.getIntro())
        introText.addActionListener(new AbstractAction(){
          override def actionPerformed(event:ActionEvent){
            level.setIntro(introText.getText())
          }
        });

        val introFile = levelEngine.find("intro-pick").asInstanceOf[JButton]
        introFile.addActionListener(new AbstractAction(){
          override def actionPerformed(event:ActionEvent){
            val chooser = new RelativeFileChooser(NewEditor.this, "Choose a file", Data.getDataPath());
            val ret = chooser.open();
            if (ret == RelativeFileChooser.OK ){
              val path = chooser.getPath();
              level.setIntro(path);
              introText.setText(path);
            }
          }
        });

        val endingText = levelEngine.find("ending").asInstanceOf[JTextField]
        endingText.setText(level.getEnding())
        endingText.addActionListener(new AbstractAction(){
          override def actionPerformed(event:ActionEvent){
            level.setEnding(endingText.getText())
          }
        });

        val endingFile = levelEngine.find("ending-pick").asInstanceOf[JButton]
        endingFile.addActionListener(new AbstractAction(){
          override def actionPerformed(event:ActionEvent){
            val chooser = new RelativeFileChooser(NewEditor.this, "Choose a file", Data.getDataPath());
            val ret = chooser.open();
            if (ret == RelativeFileChooser.OK ){
              val path = chooser.getPath();
              level.setEnding(path);
              endingText.setText(path);
            }
          }
        });

        /* initialize all the other crap for a level */
        def loadLevelProperties(level:Level){
          levelMinZ.setValue( new Integer( level.getMinZ() ) );
          levelMaxZ.setValue( new Integer( level.getMaxZ() ) );
          levelBackground.setText( level.getBackgroundFile() );
          frontPanelsData.clear();
          frontPanelsData.addAll(level.getFrontPanelNames().asInstanceOf[java.util.List[String]]);
          frontPanels.setListData( frontPanelsData );
          backPanelsData.clear();
          backPanelsData.addAll(level.getBackPanelNames().asInstanceOf[java.util.List[String]]);
          backPanels.setListData( backPanelsData );

          setOrderText();
          comboModel.update();
        };

        /* mess around with layout nonsense */
        val layout = new GridBagLayout();
        viewContainer.setLayout(layout);
        val constraints = new GridBagConstraints();
        constraints.fill = GridBagConstraints.BOTH;

        constraints.weightx = 1;
        constraints.weighty = 1;
        layout.setConstraints(viewScroll, constraints);
        view.setBorder(BorderFactory.createLineBorder(new Color(255, 0, 0)));
        viewContainer.add(viewScroll);

        def setupBlocks(level:Level){
          def editBlockProperties(block:Block, done: () => Unit){
            val dialog = new JDialog(NewEditor.this, "Edit");
            dialog.setSize(200, 200);
            val engine = new SwingEngine("block.xml");
            dialog.getContentPane().add(engine.getRootComponent().asInstanceOf[JPanel]);

            val length = engine.find( "length" ).asInstanceOf[JTextField];
            val finish = engine.find( "finish" ).asInstanceOf[JTextField];
            val isFinish = engine.find( "is-finish" ).asInstanceOf[JCheckBox];
            val id = engine.find("id").asInstanceOf[JSpinner];
            val isContinuous = engine.find( "is-continuous" ).asInstanceOf[JCheckBox];
            val save = engine.find( "save" ).asInstanceOf[JButton];
            val close = engine.find( "close" ).asInstanceOf[JButton];

            length.setText(block.getLength().toString)
            isContinuous.setSelected(block.isContinuous());
            id.setValue(block.getId());
            isFinish.setSelected(block.isFinish());
            finish.setEnabled(block.isFinish());
            finish.setText(block.getFinish().toString);
            isFinish.addActionListener( new AbstractAction(){
              override def actionPerformed(event:ActionEvent){
                finish.setEnabled(isFinish.isSelected());
              }
            });

          save.addActionListener( new AbstractAction(){
              override def actionPerformed(event:ActionEvent){
                block.setLength(java.lang.Integer.parseInt(length.getText()))
                block.setId(id.getValue().asInstanceOf[java.lang.Integer].intValue());
                if (isFinish.isSelected()){
                  block.setFinish(java.lang.Integer.parseInt(finish.getText()));
                } else {
                  block.setFinish(0);
                }
                block.setContinuous(isContinuous.isSelected());
                done()
                dialog.setVisible(false);
              }
            });

          close.addActionListener( new AbstractAction(){
              override def actionPerformed(event:ActionEvent){
                dialog.setVisible(false);
              }
            });

          dialog.setVisible( true );
        }

        def scrollToBlock(block:Block){
          var length = 0;
          val breaks = new scala.util.control.Breaks
          import breaks.{break, breakable}
          breakable{
            for (next <- toScalaList(level.getBlocks().asInstanceOf[java.util.List[Block]])){
                if (next == block ){
                  break;
                }
                if (next.isEnabled() ){
                  length += next.getLength();
                }
              }
            }
            smoothScroll(viewScroll.getHorizontalScrollBar(), viewScroll.getHorizontalScrollBar().getValue(), (length * level.getScale() - 15).toInt);
            // viewScroll.getHorizontalScrollBar().setValue( (int)(length * level.getScale() - 10) );
          }

          blocks.removeAll();
          var n = 1;
          var total = 0;
          for (block <- toScalaList(level.getBlocks().asInstanceOf[java.util.List[Block]])){
              val stuff = Box.createHorizontalBox();
              val check = new JCheckBox( new AbstractAction(){
                  override def actionPerformed(event:ActionEvent){
                    val source = event.getSource().asInstanceOf[JCheckBox];
                    block.setEnabled(source.isSelected());
                    view.revalidate();
                    viewScroll.repaint();
                  }
                });

              check.setToolTipText("Check this box to make the block appear in the game");

              check.setSelected(true);
              stuff.add(check);
              val button = new JButton("Block " + n + " : " + block.getLength());
              button.addActionListener( new AbstractAction(){
                  override def actionPerformed(event:ActionEvent){
                    if (block.isEnabled()){
                      scrollToBlock(block);
                      objectList.setBlock(block);
                    }
                  }
                });
              stuff.add(button);
              stuff.add(Box.createHorizontalStrut(3));

              val edit = new JButton( "Edit" );
              val xnum = n;
              edit.addActionListener( new AbstractAction(){
                  override def actionPerformed(event:ActionEvent){
                    editBlockProperties(block, () => {
                        button.setText( "Block " + xnum + " : " + block.getLength() );
                        view.revalidate();
                        viewScroll.repaint();
                      })
                  }
                });
              stuff.add(edit);
              stuff.add(Box.createHorizontalStrut(3));

              val erase = new JButton("Delete");
              erase.addActionListener( new AbstractAction(){
                  override def actionPerformed(event:ActionEvent){
                    mousey.setSelected(null);
                    objectList.setBlock(null);
                    level.getBlocks().remove(block);
                    setupBlocks(level)
                    view.repaint();
                  }
                });
              stuff.add(erase);

              stuff.add(Box.createHorizontalGlue());
              blocks.add(stuff);

              total += block.getLength();
              n += 1;
            }

            blocks.add(Box.createVerticalGlue());
            val addf = Box.createHorizontalBox()
            val add = new JButton( "Add block" );
            add.addActionListener( new AbstractAction(){
                override def actionPerformed(event:ActionEvent){
                  val b = new Block();
                  level.getBlocks().asInstanceOf[java.util.List[Block]].add(b);
                  setupBlocks(level)
                  view.revalidate();
                  viewScroll.repaint();
                }
              });
            addf.add(add);
            addf.add(Box.createHorizontalGlue());
            blocks.add(addf);
            val f = Box.createHorizontalBox();
            f.add(new JLabel("Total length " + total));
            f.add(Box.createHorizontalGlue());
            blocks.add(f);
            blocks.revalidate();
            blocks.repaint();
        }

        setupBlocks(level)
        loadLevelProperties(level);

        val scroll = engine.find( "level-scale" ).asInstanceOf[JSlider];
        val scale = engine.find( "scale" ).asInstanceOf[JLabel];
        scroll.addChangeListener( new ChangeListener(){
            override def stateChanged(event:ChangeEvent){
                level.setScale(scroll.getValue().toDouble * 2.0 / scroll.getMaximum());
                scale.setText("Scale: " + level.getScale());
                view.revalidate();
                viewScroll.repaint();
            }
        });

        val split = engine.getRootComponent().asInstanceOf[JSplitPane]
        split.setContinuousLayout(true)
        split
    }

    /* FIXME: make this search in data for any .txt file that matches a
     * (character) or (item) ... definition
     */
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

  def getCopy():Thing = {
    copy
  }

  def setCopy(thing:Thing){
    copy = thing
  }

  def smoothScroll(scroll:JScrollBar, starting:Int, end:Int){
    new Thread(){
      override def run(){
        var begin:Int = starting;
        for (index <- 0 to 5){
          val to = (begin + end) / 2;
          scroll.setValue(to);
          begin = to;
          try{
            Thread.sleep(20);
          } catch {
            case e:Exception => {}
          }
        }
        scroll.setValue(end);
      }
    }.start();
  }

  def generateBoysName():String = {
    return new RandomNameAction("boys.txt"){
        override def actionPerformed(event:ActionEvent){
        }
    }.generateName();
  }

  def generateGirlsName():String = {
    return new RandomNameAction( "girls.txt" ){
      override def actionPerformed(event:ActionEvent){
      }
    }.generateName();
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
    System.out.println("Current working directory is " + System.getProperty("user.dir"))
    val editor = new NewEditor();
    SwingUtilities.invokeLater(new Runnable(){
        def run(){
          editor.setVisible(true);
        }
      });
  }
}
