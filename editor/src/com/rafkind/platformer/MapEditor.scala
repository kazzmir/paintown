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

        //val maps = new HashMap[Component, World]();

        /*def doSave(level:Level, file:File){
            val out = new FileOutputStream(file);
            new PrintStream(out).print(level.toToken().toString() + "\n");
            out.close();
            System.out.println(level.toToken().toString());
        }*/

        newMap.addActionListener(new AbstractAction(){
            override def actionPerformed(event:ActionEvent){
                SwingUtilities.invokeLater(new Runnable(){
                    def run(){
                        /*val level = new Level();
                        for (i <- 1 to 3){
                            level.getBlocks().asInstanceOf[java.util.List[Block]].add(new Block());
                        }

                        levels.put(tabbed.add(createEditPanel(level)), level);*/
                        tabbed.add("New Map", createTab());
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
              /*levels.remove( tabbed.getSelectedComponent() );*/
              
              tabbed.remove( tabbed.getSelectedComponent() );
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
                    val f = chooser.getSelectedFile();
                    try{
                        /*val level = new Level(f);
                        levels.put(tabbed.add(f.getName(), createEditPanel(level)), level);*/
                        tabbed.add(f.getName(), createTab());
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
    
    def createTab():JComponent = {
        val engine = new SwingEngine("main.xml");

        val viewContainer = engine.find( "view" ).asInstanceOf[JPanel];
        val viewScroll = new JScrollPane(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS, ScrollPaneConstants.HORIZONTAL_SCROLLBAR_ALWAYS);

        val view = new JPanel(){
            override def getPreferredSize():Dimension = {
                //level.getSize()
                new Dimension(32,32);
            }

            override def paintComponent(g:Graphics){
                val h = viewScroll.getHorizontalScrollBar();
                val v = viewScroll.getVerticalScrollBar();
                g.setColor(new Color(64, 64, 64));
                //g.fillRect(0, 0, level.getWidth().toInt, v.getVisibleAmount());
                //g.clearRect(0, v.getVisibleAmount().toInt + 1, level.getWidth().toInt, level.getHeight().toInt);
                //level.render(g.asInstanceOf[Graphics2D], h.getValue(), 0, h.getVisibleAmount(), v.getVisibleAmount());
            }
        };

        //viewScroll.setPreferredSize(new Dimension(200, 200));
        //viewScroll.setViewportView(view);

        /* this allows smooth scrolling of the level */
        //viewScroll.getViewport().setScrollMode(JViewport.BACKINGSTORE_SCROLL_MODE);

        /*
           System.out.println( "JViewport.BLIT_SCROLL_MODE = " + JViewport.BLIT_SCROLL_MODE );
           System.out.println( "JViewport.BACKINGSTORE_SCROLL_MODE = " + JViewport.BACKINGSTORE_SCROLL_MODE );
           System.out.println( "JViewport.SIMPLE_SCROLL_MODE = " + JViewport.SIMPLE_SCROLL_MODE );
           System.out.println( "View scroll mode: " + viewScroll.getViewport().getScrollMode() );
           */
        //viewScroll.getHorizontalScrollBar().setBackground(new Color(128, 255, 0));
        
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
