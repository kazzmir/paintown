package com.rafkind.platformer

import java.awt._
import java.awt.image._
import javax.swing._
import java.io._
import javax.imageio._
import java.awt.event._
import javax.swing.event._
import org.swixml.SwingEngine

import com.rafkind.paintown.exception.LoadException
import com.rafkind.paintown.TokenReader
import com.rafkind.paintown.Token
import com.rafkind.paintown.MaskedImage

class World(loadfile:File){
    
    if (loadfile != null){
        load(loadfile)
    }
    
    var offsetX:Int = 15
    var offsetY:Int = 15
    
    var name:String = "No name assigned."
    var width:Int = 640
    var height:Int = 480
    var scale:Double = 2
    var _path = loadfile
    
    def this() = this(null)
    def path = _path
    def path_= (value:File):Unit = _path = value
    
    //! Resolution of map (upscaled/downscaled according to paintowns screensize)
    var resolutionX:Int = 320
    var resolutionY:Int = 240
    
    //! mechanics
    var gravityX:Double = 0
    var gravityY:Double = 0
    var acceleration:Double = 0
    
    //! Animations
    var animations = scala.collection.mutable.HashMap.empty[String, Animation]
    
    //! Tilesets
    var tilesets = scala.collection.mutable.ArrayBuffer.empty[TileSet]
    tilesets += new TileSet()
    
    def render(g:Graphics2D, x:Int, y:Int, width:Int, height:Int) = {
        g.scale(scale, scale)
        
        // Entire map
        g.setColor( new Color( 230, 230, 250 ) )
        g.fillRect(offsetX, offsetY, this.width, this.height)
        
        // Tilesets
        tilesets.foreach{
            case (tileset) => tileset.render(g, offsetX, offsetY)
        }
        
        // Viewport resolution
        g.setColor( new Color( 0, 0, 255 ) )
        g.drawRect(offsetX, offsetY, this.resolutionX, this.resolutionY)
    }

    def load(f:File) = {
        val reader = new TokenReader( f )
        val head = reader.nextToken()
        if ( ! head.getName().equals( "world" ) ){
            throw new LoadException( "Starting token is not 'world'" )
        }

        System.out.println( "Loaded " + f )
    }

    def loadImage(s:String):Image = {
        try{
            MaskedImage.load( MapEditor.getDataPath( s ).getPath() )
        } catch {
            case ex:IOException => throw new LoadException( "Could not load " + s )
        }
    }

    def getSize():Dimension = {
        val dimension = new Dimension()
        dimension.setSize((width * scale) + (offsetX*2), (height * scale) + (offsetY*2))
        dimension
    }

    def toToken():Token = {
        val world = new Token()
        world.addToken( new Token( world, "world" ) )
        

        world
    }
    
    def createPanel(view:JPanel, viewScroll:JScrollPane, tabbed:JTabbedPane):JPanel = {
        val engine = new SwingEngine( "platformer/world.xml" )
        val pane = engine.getRootComponent().asInstanceOf[JPanel]
        
        {
            val nameField = engine.find("name").asInstanceOf[JTextField]
            nameField.setText(name)
            nameField.getDocument().addDocumentListener(new DocumentListener() {
                def changedUpdate(e:DocumentEvent) = {
                    update()
                }
                def removeUpdate(e:DocumentEvent) = {
                    update()
                }
                def insertUpdate(e:DocumentEvent) = {
                    update()
                }
                
                def update() = {
                    name = nameField.getText()
                    tabbed.setTitleAt(tabbed.getSelectedIndex(), nameField.getText())
                }
            })
        }
        
        {
            val resolution = engine.find("resolution-x").asInstanceOf[JSpinner]
            val model = new SpinnerNumberModel()
            resolution.setModel(model)
            model.setValue(resolutionX)
            model.setStepSize(16)
            resolution.addChangeListener(new ChangeListener(){
                override def stateChanged(event:ChangeEvent){
                    val spinner = event.getSource().asInstanceOf[JSpinner]
                    val i = spinner.getValue().asInstanceOf[java.lang.Integer]
                    resolutionX = i.intValue()
                    viewScroll.repaint()
                }
            })
        }
        
        {
            val resolution = engine.find("resolution-y").asInstanceOf[JSpinner]
            val model = new SpinnerNumberModel()
            resolution.setModel(model)
            model.setValue(resolutionY)
            model.setStepSize(16)
            resolution.addChangeListener(new ChangeListener(){
                override def stateChanged(event:ChangeEvent){
                    val spinner = event.getSource().asInstanceOf[JSpinner]
                    val i = spinner.getValue().asInstanceOf[java.lang.Integer]
                    resolutionY = i.intValue()
                    viewScroll.repaint()
                }
            })
        }
        
        {
            val dimensions = engine.find("dimensions-x").asInstanceOf[JSpinner]
            val model = new SpinnerNumberModel()
            dimensions.setModel(model)
            model.setValue(width)
            model.setStepSize(16)
            dimensions.addChangeListener(new ChangeListener(){
                override def stateChanged(event:ChangeEvent){
                    val spinner = event.getSource().asInstanceOf[JSpinner]
                    val i = spinner.getValue().asInstanceOf[java.lang.Integer]
                    width = i.intValue()
                    view.revalidate()
                    viewScroll.repaint()
                }
            })
        }
        
        {
            val dimensions = engine.find("dimensions-y").asInstanceOf[JSpinner]
            val model = new SpinnerNumberModel()
            dimensions.setModel(model)
            model.setValue(height)
            model.setStepSize(16)
            dimensions.addChangeListener(new ChangeListener(){
                override def stateChanged(event:ChangeEvent){
                    val spinner = event.getSource().asInstanceOf[JSpinner]
                    val i = spinner.getValue().asInstanceOf[java.lang.Integer]
                    height = i.intValue()
                    view.revalidate()
                    viewScroll.repaint()
                }
            })
        }
        
        {
            val gravity = engine.find("gravity-x").asInstanceOf[JSpinner]
            gravity.setModel(new SpinnerNumberModel(gravityX, -2000.0, 2000.0, 0.001))
            gravity.addChangeListener(new ChangeListener(){
                override def stateChanged(event:ChangeEvent){
                    val spinner = event.getSource().asInstanceOf[JSpinner]
                    val i = spinner.getValue().asInstanceOf[java.lang.Double]
                    gravityX = i.doubleValue()
                }
            })
        }
        
        {
            val gravity = engine.find("gravity-y").asInstanceOf[JSpinner]
            gravity.setModel(new SpinnerNumberModel(gravityY, -2000.0, 2000.0, 0.001))
            gravity.addChangeListener(new ChangeListener(){
                override def stateChanged(event:ChangeEvent){
                    val spinner = event.getSource().asInstanceOf[JSpinner]
                    val i = spinner.getValue().asInstanceOf[java.lang.Double]
                    gravityY = i.doubleValue()
                }
            })
        }
        
        {
            val accelerationField = engine.find("acceleration").asInstanceOf[JSpinner]
            accelerationField.setModel(new SpinnerNumberModel(acceleration, -2000.0, 2000.0, 0.001))
            accelerationField.addChangeListener(new ChangeListener(){
                override def stateChanged(event:ChangeEvent){
                    val spinner = event.getSource().asInstanceOf[JSpinner]
                    val i = spinner.getValue().asInstanceOf[java.lang.Double]
                    acceleration = i.doubleValue()
                }
            })
        }
        
        pane
    }
}
