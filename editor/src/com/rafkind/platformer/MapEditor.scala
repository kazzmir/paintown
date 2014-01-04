package com.rafkind.platformer

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
import com.rafkind.paintown.Closer
import com.rafkind.paintown.Data
import com.rafkind.paintown.Lambda0
import com.rafkind.paintown.Lambda1
import com.rafkind.paintown.Lambda2
import com.rafkind.paintown.Token
import com.rafkind.paintown.TokenReader
import com.rafkind.paintown.RelativeFileChooser
import com.rafkind.paintown.level.EditorException
import javax.swing.filechooser.FileFilter

class Editor extends JFrame("Platformer Map Editor"){
    
    var tabbed:JTabbedPane = null
    
    construct()

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
                     (screen.getHeight() * 0.9).toInt)

        Closer.open()
        val menuBar = new JMenuBar()

        val menuFile = new JMenu("File")

        val newWorld = new JMenuItem("New World")
        menuFile.add(newWorld)
        val loadWorld = new JMenuItem("Open World")
        menuFile.add(loadWorld)
        val saveWorld = new JMenuItem("Save World")
        menuFile.add(saveWorld)
        val saveWorldAs = new JMenuItem("Save World As")
        menuFile.add(saveWorldAs)
        val closeWorld = new JMenuItem("Close World")
        menuFile.add(closeWorld)

        val data = new JMenuItem( "Data path" )
        menuFile.add(data)
        val exit = new JMenuItem( "Exit" )
        menuFile.add(exit)

        // Add to menuBar
        menuBar.add(menuFile)

        menuFile.setMnemonic( KeyEvent.VK_F )
        data.setMnemonic( KeyEvent.VK_D )
        exit.setMnemonic( KeyEvent.VK_E )
        exit.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Q, Event.CTRL_MASK))
        newWorld.setMnemonic( KeyEvent.VK_N )
        newWorld.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_N, Event.CTRL_MASK))
        saveWorld.setMnemonic( KeyEvent.VK_S )
        saveWorld.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, Event.CTRL_MASK))
        saveWorldAs.setMnemonic( KeyEvent.VK_A )
        saveWorldAs.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_A, Event.CTRL_MASK))
        loadWorld.setMnemonic( KeyEvent.VK_O )
        loadWorld.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, Event.CTRL_MASK))
        closeWorld.setMnemonic( KeyEvent.VK_W )
        closeWorld.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_W, Event.CTRL_MASK))

        tabbed = new JTabbedPane()
        this.getContentPane().add(tabbed)

        exit.addActionListener(new ActionListener(){
            override def actionPerformed(event:ActionEvent){
              Closer.close()
            }
          })

        data.addActionListener(new ActionListener(){
            override def actionPerformed(event:ActionEvent){
              /* just a container for an object */
              class ObjectBox {
                var _internal:Object = null

                def internal = _internal 

                def internal_= (obj:Object):Unit = _internal = obj
              }

              val engine = new SwingEngine("data-path.xml")
              val path = engine.find("path").asInstanceOf[JTextField]
              val box = new ObjectBox()
              box.internal = Data.getDataPath()
              path.setText(Data.getDataPath().getPath())
              val change = engine.find("change").asInstanceOf[JButton]
              change.addActionListener(new AbstractAction(){
                  override def actionPerformed(event:ActionEvent){
                    val chooser = new JFileChooser(new File("."))	
                    chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY)
                    val returnVal = chooser.showOpenDialog(Editor.this)
                    if (returnVal == JFileChooser.APPROVE_OPTION){
                      val newPath = chooser.getSelectedFile()
                      path.setText(newPath.getPath())
                      box.internal = newPath
                    }
                  }
                })
              val save = engine.find("save").asInstanceOf[JButton]
              val cancel = engine.find("cancel").asInstanceOf[JButton]
              val dialog = new JDialog(Editor.this, "Paintown data path")
              save.addActionListener(new AbstractAction(){
                  override def actionPerformed(event:ActionEvent){
                    Data.setDataPath(box.internal.asInstanceOf[File])
                    dialog.setVisible(false)
                  }
                })
              cancel.addActionListener(new AbstractAction(){
                  override def actionPerformed(event:ActionEvent){
                    dialog.setVisible(false)
                  }
                })
              val panel = engine.getRootComponent().asInstanceOf[JPanel]
              dialog.getContentPane().add(panel)
              dialog.setSize(300, 300)
              dialog.setVisible(true)
            }
        })

        val worlds = new HashMap[Component, World]()

        def doSave(world:World, file:File){
            val out = new FileOutputStream(file)
            new PrintStream(out).print(world.toToken().toString() + "\n")
            out.close()
            System.out.println(world.toToken().toString())
        }

        newWorld.addActionListener(new AbstractAction(){
            override def actionPerformed(event:ActionEvent){
                SwingUtilities.invokeLater(new Runnable(){
                    def run(){
                        val world = new World()
                        
                        world.name = "New World"
                        
                        var counter = 0
                        
                        var i = 0
                        for (i <- 0 until tabbed.getTabCount()){
                            try {
                                var w = worlds.get(tabbed.getComponentAt(i)).asInstanceOf[World]
                                if (w.name contains "New World"){
                                    counter+=1
                                }
                            } catch {
                                case fail:ClassCastException => {}
                            }
                        }
                        
                        var newTabName = "New World"
                        if (counter > 0){
                            newTabName += "(" + counter + ")"
                        }

                        worlds.put(tabbed.add(newTabName, createTab(world)), world)
                    }
                })
            }
        })

        saveWorld.addActionListener(new AbstractAction(){
            override def actionPerformed(event:ActionEvent){
                if (tabbed.getSelectedComponent() != null){
                    val world = worlds.get(tabbed.getSelectedComponent()).asInstanceOf[World]
                    val file:File = world.path match {
                      case null => userSelectFile("Save Level")
                      case what => what
                    }

                    /* write the text to a file */
                    if (file != null){
                        try{
                            doSave(world, file)
                            world.path = file
                            tabbed.setTitleAt(tabbed.getSelectedIndex(), file.getName())
                        } catch {
                            case fail:Exception => {
                              fail.printStackTrace()
                              showError("Could not save " + file + " because " + fail.getMessage())
                            }
                        }
                    }
                }
            }
        })

        saveWorldAs.addActionListener(new AbstractAction(){
          override def actionPerformed(event:ActionEvent){
            if (tabbed.getSelectedComponent() != null){
              val world = worlds.get(tabbed.getSelectedComponent()).asInstanceOf[World]
              val file = userSelectFile("Save level as")
              
              /* write the text to a file */
              if (file != null){
                try{
                  doSave(world, file)
                  world.path = file
                  tabbed.setTitleAt(tabbed.getSelectedIndex(), file.getName())
                } catch {
                  case fail:Exception => {
                    fail.printStackTrace()
                    showError("Could not save " + file + " because " + fail.getMessage() )
                  }
                }
              }
            }
          }
        })

        closeWorld.addActionListener( new AbstractAction(){
          override def actionPerformed(event:ActionEvent){
            if ( tabbed.getSelectedComponent() != null ){
              worlds.remove(tabbed.getSelectedComponent())
              tabbed.remove(tabbed.getSelectedComponent())
            }
          }
        })

        loadWorld.addActionListener( new AbstractAction(){
            override def actionPerformed(event:ActionEvent){
                val chooser = new JFileChooser( new File( "." ) )	
                chooser.setFileFilter(new FileFilter(){
                    def accept(f:File):Boolean = {
                        f.isDirectory() || f.getName().endsWith( ".txt" )
                    }

                    def getDescription():String = {
                        "World files"
                    }
                })

                val returnVal = chooser.showOpenDialog(Editor.this)
                if ( returnVal == JFileChooser.APPROVE_OPTION ){
                    val f = chooser.getSelectedFile()
                    try{
                        val world = new World(f)
                        worlds.put(tabbed.add(f.getName(), createTab(world)), world)
                    } catch {
                        case fail:LoadException => {
                          showError( "Could not load " + f.getName() )
                          System.out.println( "Could not load " + f.getName() )
                          fail.printStackTrace()
                        }
                    }
                }
          }
        })

        this.setJMenuBar(menuBar)

        this.addWindowListener(new WindowAdapter{
            override def windowClosing(e:WindowEvent){
                Closer.close()
            }
        })
    }
    
    def createTab(world:World):JComponent = {
        val engine = new SwingEngine("platformer/main.xml")

        val viewContainer = engine.find( "view" ).asInstanceOf[JPanel]
        val viewScroll = new JScrollPane(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS, ScrollPaneConstants.HORIZONTAL_SCROLLBAR_ALWAYS)

        val view = new JPanel(){
            override def getPreferredSize():Dimension = {
                world.getSize()
            }

            override def paintComponent(g:Graphics){
                val h = viewScroll.getHorizontalScrollBar()
                val v = viewScroll.getVerticalScrollBar()
                g.setColor(new Color(64, 64, 64))
                g.fillRect(0, 0, this.getWidth(), this.getHeight())
                //g.clearRect(0, v.getVisibleAmount().toInt + 1, world.getWidth().toInt, world.getHeight().toInt)
                world.render(g.asInstanceOf[Graphics2D], h.getValue(), 0, h.getVisibleAmount(), v.getVisibleAmount())
            }
        }

        viewScroll.setViewportView(view)
        view.revalidate()

        /* this allows smooth scrolling of the level */
        //viewScroll.getViewport().setScrollMode(JViewport.BACKINGSTORE_SCROLL_MODE)

        viewScroll.getHorizontalScrollBar().setBackground(new Color(128, 255, 0))
        
        class Mouser extends MouseMotionAdapter with MouseInputListener {
            var dx:Double = 0
            var dy:Double = 0
            var sx:Double = 0
            var sy:Double = 0
            var currentPopup:JDialog = null
            val leftMask = InputEvent.BUTTON1_DOWN_MASK
            val rightMask = InputEvent.BUTTON3_DOWN_MASK

            override def mouseDragged(event:MouseEvent){
                viewScroll.repaint()
                val x = (event.getX() / world.scale) - world.offsetX
                val y = (event.getY() / world.scale) - world.offsetY
                //System.out.println("Dragging mouse through (" + x + "," + y + ")")
                if ((event.getModifiersEx() & (leftMask | rightMask)) == leftMask){
                    // Left button
                    world.addTile(x.intValue(), y.intValue())
                } else if ((event.getModifiersEx() & (leftMask | rightMask)) == rightMask){
                    // Right button
                    world.removeTile(x.intValue(), y.intValue())
                }
            }

            def leftClick(event:MouseEvent):Boolean = {
                event.getButton() == MouseEvent.BUTTON1
            }

            def rightClick(event:MouseEvent):Boolean = {
                event.getButton() == MouseEvent.BUTTON3
            }

            def findFiles(dir:File, ending:String):List[File] = {
                val all = dir.listFiles(new java.io.FileFilter(){
                    override def accept(path:File):Boolean = {
                        path.isDirectory() || path.getName().endsWith(ending)
                    }
                })
                var files = List[File]()
                for (file <- all){
                    if (file.isDirectory() ){
                        files = files ++ findFiles(file, ending)
                    } else {
                        files = files :+ file
                    }
                }
                return files
            }

            def mousePressed(event:MouseEvent){
                val x = (event.getX() / world.scale) - world.offsetX
                val y = (event.getY() / world.scale) - world.offsetY
                if (leftClick(event)){
                    //System.out.println("Had left click at (" + x + "," + y + ")")
                    world.addTile(x.intValue(), y.intValue())
                } else if (rightClick(event)){
                    //System.out.println("Had right click at (" + x + "," + y + ")")
                    world.removeTile(x.intValue(), y.intValue())
                }
            }

            def mouseExited(event:MouseEvent){
                viewScroll.repaint()
            }

            def mouseClicked(event:MouseEvent){
            }

            def mouseEntered(event:MouseEvent){
            }

            def mouseReleased(event:MouseEvent){
                viewScroll.repaint()
            }
        }

        val mousey = new Mouser()

        view.addMouseMotionListener(mousey )
        view.addMouseListener(mousey )
        view.addMouseListener(new MouseAdapter(){
            override def mousePressed(event:MouseEvent){
                /* force focus to move to the view */
                view.requestFocusInWindow() 
            }
        })
        
        /* grid layout */
        val layout = new GridBagLayout()
        viewContainer.setLayout(layout)
        val constraints = new GridBagConstraints()
        constraints.fill = GridBagConstraints.BOTH

        constraints.weightx = 1
        constraints.weighty = 1
        layout.setConstraints(viewScroll, constraints)
        
        viewContainer.add(viewScroll)
        
        // Create panel to handle world values
        val values = engine.find("values").asInstanceOf[JPanel]
        values.add(world.createDetailsPanel(view, viewScroll, tabbed))
        val screenHeight = Toolkit.getDefaultToolkit().getScreenSize().getHeight()
        values.setPreferredSize(new Dimension(200, (screenHeight - (screenHeight * .2)).intValue()))
        
        world.connectScaleOffset(engine, view, viewScroll)
        
        world.createUpdateTimer(view, viewScroll)
        
        val split = engine.getRootComponent().asInstanceOf[JSplitPane]
        split.setContinuousLayout(true)
        split.setDividerLocation(0.8)
        split
    }

    def dataPath(file:File):File = {
        new File(Data.getDataPath().getPath() + "/" + file.getPath())
    }

    def userSelectFile(title:String):File = {
        val chooser = new JFileChooser(new File("."))
        chooser.setDialogTitle(title)
        chooser.showOpenDialog(Editor.this) match {
            case JFileChooser.APPROVE_OPTION => chooser.getSelectedFile()
            case _ => null
        }
    }

    
    def showError(message:String){
        JOptionPane.showMessageDialog(null, message, "Platformer World Editor Error", JOptionPane.ERROR_MESSAGE)
    }

    def showError(message:EditorException){
        showError(message.getMessage())
    }
}

object MapEditor {
    val editor = new Editor()
    def main(args: Array[String]):Unit = {
        System.out.println("Current working directory is " + System.getProperty("user.dir"))

        SwingUtilities.invokeLater(new Runnable(){
            def run(){
              editor.setVisible(true)
            }
        })
    }
  
  def getDataPath(file:String):File = {
      editor.dataPath(new File(file))
  }
}
