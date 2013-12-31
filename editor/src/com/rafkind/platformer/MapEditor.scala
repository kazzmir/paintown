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

        val menuFile = new JMenu("File");

        val newMap = new JMenuItem("New Map");
        menuFile.add(newMap);
        val loadMap = new JMenuItem("Open Map");
        menuFile.add(loadMap);
        val saveMap = new JMenuItem("Save Map");
        menuFile.add(saveMap);
        val saveMapAs = new JMenuItem("Save Map As");
        menuFile.add(saveMapAs);
        val closeMap = new JMenuItem("Close Map");
        menuFile.add(closeMap);

        val data = new JMenuItem( "Data path" );
        menuFile.add(data);
        val exit = new JMenuItem( "Exit" );
        menuFile.add(exit);

        // Add to menuBar
        menuBar.add(menuFile);

        menuFile.setMnemonic( KeyEvent.VK_F );
        data.setMnemonic( KeyEvent.VK_D );
        exit.setMnemonic( KeyEvent.VK_E );
        exit.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Q, Event.CTRL_MASK));
        newMap.setMnemonic( KeyEvent.VK_N );
        newMap.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_N, Event.CTRL_MASK));
        saveMap.setMnemonic( KeyEvent.VK_S );
        saveMap.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, Event.CTRL_MASK));
        saveMapAs.setMnemonic( KeyEvent.VK_A );
        saveMapAs.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_A, Event.CTRL_MASK));
        loadMap.setMnemonic( KeyEvent.VK_O );
        loadMap.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, Event.CTRL_MASK));
        closeMap.setMnemonic( KeyEvent.VK_W );
        closeMap.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_W, Event.CTRL_MASK));

        val tabbed = new JTabbedPane();
        this.getContentPane().add(tabbed);

        exit.addActionListener(new ActionListener(){
            override def actionPerformed(event:ActionEvent){
              Closer.close();
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
                    val returnVal = chooser.showOpenDialog(Editor.this);
                    if (returnVal == JFileChooser.APPROVE_OPTION){
                      val newPath = chooser.getSelectedFile();
                      path.setText(newPath.getPath());
                      box.set(newPath);
                    }
                  }
                });
              val save = engine.find("save").asInstanceOf[JButton];
              val cancel = engine.find("cancel").asInstanceOf[JButton];
              val dialog = new JDialog(Editor.this, "Paintown data path");
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

        val worlds = new HashMap[Component, World]();

        def doSave(world:World, file:File){
            val out = new FileOutputStream(file);
            new PrintStream(out).print(world.toToken().toString() + "\n");
            out.close();
            System.out.println(world.toToken().toString());
        }

        newMap.addActionListener(new AbstractAction(){
            override def actionPerformed(event:ActionEvent){
                SwingUtilities.invokeLater(new Runnable(){
                    def run(){
                        val world = new World()

                        worlds.put(tabbed.add("New World", createTab(world)), world)
                    }
                });
            }
        });

        saveMap.addActionListener(new AbstractAction(){
            override def actionPerformed(event:ActionEvent){
                if (tabbed.getSelectedComponent() != null){
                    /*val level = levels.get(tabbed.getSelectedComponent()).asInstanceOf[Level];
                    val file:File = level.getPath() match {
                      case null => userSelectFile("Save Level");
                      case what => what
                    }*/

                    /* write the text to a file */
                    /*if (file != null){
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
                    }*/
                }
            }
        });

        saveMapAs.addActionListener(new AbstractAction(){
          override def actionPerformed(event:ActionEvent){
            if (tabbed.getSelectedComponent() != null){
              /*val level = levels.get(tabbed.getSelectedComponent()).asInstanceOf[Level];
              val file = userSelectFile("Save level as");*/
              
              /* write the text to a file */
              /*if (file != null){
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
              }*/
            }
          }
        });

        closeMap.addActionListener( new AbstractAction(){
          override def actionPerformed(event:ActionEvent){
            if ( tabbed.getSelectedComponent() != null ){
              worlds.remove(tabbed.getSelectedComponent())
              tabbed.remove(tabbed.getSelectedComponent())
            }
          }
        });

        loadMap.addActionListener( new AbstractAction(){
            override def actionPerformed(event:ActionEvent){
                val chooser = new JFileChooser( new File( "." ) );	
                chooser.setFileFilter(new FileFilter(){
                    def accept(f:File):Boolean = {
                        f.isDirectory() || f.getName().endsWith( ".txt" );
                    }

                    def getDescription():String = {
                        "Map files";
                    }
                });

                val returnVal = chooser.showOpenDialog(Editor.this);
                if ( returnVal == JFileChooser.APPROVE_OPTION ){
                    val f = chooser.getSelectedFile()
                    try{
                        val world = new World(f)
                        worlds.put(tabbed.add(f.getName(), createTab(world)), world)
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
    
    def createTab(world:World):JComponent = {
        val engine = new SwingEngine("main.xml");

        val viewContainer = engine.find( "view" ).asInstanceOf[JPanel];
        val viewScroll = new JScrollPane(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS, ScrollPaneConstants.HORIZONTAL_SCROLLBAR_ALWAYS);

        val view = new JPanel(){
            override def getPreferredSize():Dimension = {
                world.getSize()
            }

            override def paintComponent(g:Graphics){
                val h = viewScroll.getHorizontalScrollBar()
                val v = viewScroll.getVerticalScrollBar()
                g.setColor(new Color(64, 64, 64))
                g.fillRect(0, 0, world.getWidth().toInt, v.getVisibleAmount());
                g.clearRect(0, v.getVisibleAmount().toInt + 1, world.getWidth().toInt, world.getHeight().toInt);
                world.render(g.asInstanceOf[Graphics2D], h.getValue(), 0, h.getVisibleAmount(), v.getVisibleAmount())
                System.out.println("Painting..");
            }
        };

        viewScroll.setPreferredSize(new Dimension(200, 200))
        viewScroll.setViewportView(view)

        /* this allows smooth scrolling of the level */
        viewScroll.getViewport().setScrollMode(JViewport.BACKINGSTORE_SCROLL_MODE)

        viewScroll.getHorizontalScrollBar().setBackground(new Color(128, 255, 0));
        
        class Mouser extends MouseMotionAdapter with MouseInputListener {
            var dx:Double = 0
            var dy:Double = 0
            var sx:Double = 0
            var sy:Double = 0
            var currentPopup:JDialog = null

            override def mouseDragged(event:MouseEvent){
                viewScroll.repaint();
            }

            def leftClick(event:MouseEvent):Boolean = {
                event.getButton() == MouseEvent.BUTTON1;
            }

            def rightClick(event:MouseEvent):Boolean = {
                event.getButton() == MouseEvent.BUTTON3;
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

            def mousePressed(event:MouseEvent){
                if (leftClick(event)){
                    
                } else if (rightClick(event)){
                    
                }
            }

            def mouseExited(event:MouseEvent){
                viewScroll.repaint();
            }

            def mouseClicked(event:MouseEvent){
            }

            def mouseEntered(event:MouseEvent){
            }

            def mouseReleased(event:MouseEvent){
                viewScroll.repaint();
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
        
        
        val scroll = engine.find( "level-scale" ).asInstanceOf[JSlider];
        val scale = engine.find( "scale" ).asInstanceOf[JLabel];
        scroll.addChangeListener( new ChangeListener(){
            override def stateChanged(event:ChangeEvent){
                world.setScale(scroll.getValue().toDouble * 2.0 / scroll.getMaximum());
                scale.setText("Scale: " + world.getScale());
                view.revalidate();
                viewScroll.repaint();
            }
        });

        val split = engine.getRootComponent().asInstanceOf[JSplitPane]
        split.setContinuousLayout(true)
        split
    }

    def dataPath(file:File):File = {
        new File(Data.getDataPath().getPath() + "/" + file.getPath());
    }

    def userSelectFile(title:String):File = {
        val chooser = new JFileChooser(new File("."));
        chooser.setDialogTitle(title);
        chooser.showOpenDialog(Editor.this) match {
            case JFileChooser.APPROVE_OPTION => chooser.getSelectedFile()
            case _ => null
        }
    }

    
    def showError(message:String){
        JOptionPane.showMessageDialog(null, message, "Platformer Map Editor Error", JOptionPane.ERROR_MESSAGE);
    }

    def showError(message:EditorException){
        showError(message.getMessage());
    }
}

object MapEditor {
    val editor = new Editor();
    def main(args: Array[String]):Unit = {
        System.out.println("Current working directory is " + System.getProperty("user.dir"))

        SwingUtilities.invokeLater(new Runnable(){
            def run(){
              editor.setVisible(true);
            }
        });
    }
  
  def getDataPath(file:String):File = {
      editor.dataPath(new File(file))
  }
}
