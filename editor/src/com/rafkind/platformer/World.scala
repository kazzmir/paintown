package com.rafkind.platformer

import java.awt._
import java.awt.image._
import javax.swing._
import java.io._
import javax.imageio._
import java.awt.event._
import javax.swing.event._
import org.swixml.SwingEngine

import scala.collection.immutable.List

import com.rafkind.paintown.exception.LoadException
import com.rafkind.paintown.TokenReader
import com.rafkind.paintown.Token
import com.rafkind.paintown.MaskedImage

class AnimationUpdater(val view:JPanel, val viewScroll:JScrollPane, val animations:AnimationListModel) extends ActionListener {
    var timer = new Timer(25, this)
    timer.start()
    
    def actionPerformed(e:ActionEvent) = {
        animations.getAll().foreach{
            case (animation) => animation.update()
        }
        view.revalidate()
        viewScroll.repaint()
    }
}

class World(var _path:File){
    
    var offsetX:Int = 15
    var offsetY:Int = 15
    
    var name:String = "No name assigned."
    var width:Int = 640
    var height:Int = 480
    var scale:Double = 2
    
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
    val animations = new AnimationListModel()
    
    //! Background Tilesets
    val backgrounds = new TileSetListModel()
    
    //! Foreground Tilesets
    var foregrounds = new TileSetListModel()
    
    //! Collision maps
    var collisionMaps = new CollisionMap()
    
    //! actions (updating animations)
    var actions:AnimationUpdater = null
    
    if (_path != null){
        load(_path)
    } else {
        val tileset = new TileSet("Empty tileset")
        tileset.isCurrent = true
        backgrounds.add(tileset)
    }
    
    def render(g:Graphics2D, x:Int, y:Int, width:Int, height:Int) = {
        g.scale(scale, scale)
        
        // Entire map
        g.setColor( new Color( 230, 230, 250 ) )
        g.fillRect(offsetX, offsetY, this.width, this.height)
        
        // backgrounds
        backgrounds.getAll().foreach{
            case (tileset) => tileset.render(g, offsetX, offsetY, animations)
        }
        
        // Collision maps
        collisionMaps.render(g, offsetX, offsetY)
        
        // Objects
        
        // foregrounds
        foregrounds.getAll().foreach{
            case (tileset) => tileset.render(g, offsetX, offsetY, animations)
        }
        
        // Viewport resolution
        g.setColor( new Color( 0, 0, 255 ) )
        g.drawRect(offsetX, offsetY, this.resolutionX, this.resolutionY)
    }
    
    def addAnimation(animation:Animation) = {
        if (actions != null){
            actions.timer.stop()
        }
        
        animations.add(animation)
        
        if (actions != null){
            actions.timer.start()
        }
    }

    def load(f:File) = {
        val reader = new TokenReader( f )
        val head = reader.nextToken()
        if ( ! head.getName().equals( "world" ) ){
            throw new LoadException( "Starting token is not 'world'" )
        }
        
        val nameToken = head.findToken("name")
        if (nameToken != null){
            name = nameToken.readString(0)
        }
        val resolutionToken = head.findToken("resolution")
        if (resolutionToken != null){
            resolutionX = resolutionToken.readInt(0)
            resolutionY = resolutionToken.readInt(1)
        }
        val dimensionToken = head.findToken("dimensions")
        if (dimensionToken != null){
            width = dimensionToken.readInt(0)
            height = dimensionToken.readInt(1)
        }
        
        val mechanicsToken = head.findToken("mechanics")
        if (mechanicsToken != null){
            val gravity = mechanicsToken.findToken("gravity")
            if (gravity != null){
                gravityX = gravity.readDouble(0)
                gravityY = gravity.readDouble(1)
            }
            val accel = mechanicsToken.findToken("acceleration")
            if (accel != null){
                acceleration = accel.readDouble(0);
            }
        }
        
        val animationIterator = head.findTokens("animation").iterator()
        while(animationIterator.hasNext()){
            val animation = new Animation("Not assigned")
            animation.readToken(animationIterator.next())
            addAnimation(animation)
        }
        
        val backgroundIterator = head.findTokens("background").iterator()
        while(backgroundIterator.hasNext()){
            try {
                val tileset = new TileSet("Not assigned")
                tileset.readToken(backgroundIterator.next(), animations)
                backgrounds.add(tileset)
            } catch {
                case ex:LoadException => { println(ex.getMessage()) }
            }
        }
        
        val foregroundIterator = head.findTokens("foreground").iterator()
        while(foregroundIterator.hasNext()){
            try {
                val tileset = new TileSet("Not assigned")
                tileset.readToken(foregroundIterator.next(), animations)
                foregrounds.add(tileset)
            } catch {
                case ex:LoadException => { println(ex.getMessage()) }
            }
        }
        
        val collisionToken = head.findToken("collision-map")
        if (collisionToken != null){
            collisionMaps.readToken(collisionToken)
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
        world.addToken(Array("name", name))
        world.addToken(Array("resolution", String.valueOf(resolutionX), String.valueOf(resolutionY)))
        world.addToken(Array("dimensions", String.valueOf(width), String.valueOf(height)))
        val mechanics = new Token()
        mechanics.addToken(new Token(mechanics, "mechanics"))
        mechanics.addToken(Array("gravity", String.valueOf(gravityX), String.valueOf(gravityY)))
        world.addToken(mechanics)
        
        animations.getAll().foreach{
            case (animation) => world.addToken(animation.toToken())
        }
        
        backgrounds.getAll().foreach{
            case (tileset) => world.addToken(tileset.toToken("background"))
        }
        
        foregrounds.getAll().foreach{
            case (tileset) => world.addToken(tileset.toToken("foreground"))
        }
        
        // Collision maps
        world.addToken(collisionMaps.toToken())

        world
    }
    
    def addTile(x:Int, y:Int) = {
        val tileset = getCurrentTileSet()
        val animation = getCurrentAnimation()
        if (tileset != null && animation != null){
            val column = if (x < 0) (x / tileset.tileWidth)-1 else x / tileset.tileWidth
            val row = if (y < 0) (y / tileset.tileHeight)-1 else y / tileset.tileHeight
            //System.out.println("Adding tile to location (" + column + "," + row + ")")
            val tile = new Tile()
            tile.animation = animation
            tile.column = column
            tile.row = row
            tileset.tiles.add(tile)
        }
    }
    
    def removeTile(x:Int, y:Int) = {
        val tileset = getCurrentTileSet()
        if (tileset != null){
            val column = if (x < 0) (x / tileset.tileWidth)-1 else x / tileset.tileWidth
            val row = if (y < 0) (y / tileset.tileHeight)-1 else y / tileset.tileHeight
            tileset.tiles.remove(column, row)
        }
    }
    
    def getCurrentAnimation():Animation = {
        animations.getAll().foreach{
            case (anim) => if (anim.isCurrent) return anim
        }
        null
    }
    
    def clearCurrentAnimation() = {
        animations.getAll().foreach{
            case (anim) => if (anim.isCurrent) anim.isCurrent = false
        }
    }
    
    def getCurrentTileSet():TileSet = {
        backgrounds.getAll().foreach{
            case (tileset) => if (tileset.isCurrent) return tileset
        }
        
        foregrounds.getAll().foreach{
            case (tileset) => if (tileset.isCurrent) return tileset
        }
        null
    }
    
    def clearCurrentTileSet() = {
        backgrounds.getAll().foreach{
            case (tileset) => if (tileset.isCurrent) tileset.isCurrent = false
        }
        foregrounds.getAll().foreach{
            case (tileset) => if (tileset.isCurrent) tileset.isCurrent = false
        }
    }
    
    def createDetailsPanel(view:JPanel, viewScroll:JScrollPane, tabbed:JTabbedPane):JPanel = {
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
        
        // Animations
        {
            val anims = engine.find("anims").asInstanceOf[JList[Animation]]
            anims.setModel(animations)
            
            anims.setVisibleRowCount(4)
            
            val add = engine.find("add-anim-button").asInstanceOf[JButton]
            add.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    val animation = new Animation("New Animation")
                    addAnimation(animation)
                    animation.editDialog(view, viewScroll, anims)
                } 
            })
            
            val edit = engine.find("edit-anim-button").asInstanceOf[JButton]
            edit.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    if (animations.getSize() > 0 && anims.getSelectedIndex() != -1){
                        animations.getElementAt(anims.getSelectedIndex()).editDialog(view, viewScroll, anims)
                    }
                } 
            })
            
            val current = engine.find("current-anim-button").asInstanceOf[JButton]
            current.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    if (animations.getSize() > 0 && anims.getSelectedIndex() != -1){
                        clearCurrentAnimation()
                        animations.getElementAt(anims.getSelectedIndex()).isCurrent = true
                        view.revalidate()
                        viewScroll.repaint()
                        anims.repaint()
                    }
                } 
            })
            
            val remove = engine.find("remove-anim-button").asInstanceOf[JButton]
            remove.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    if (animations.getSize() > 0 && anims.getSelectedIndex() != -1){
                        animations.remove(anims.getSelectedIndex())
                        view.revalidate()
                        viewScroll.repaint()
                    }
                } 
            })
        }
        
        // Backgrounds and foregrounds
        {
            val bgs = engine.find("backgrounds").asInstanceOf[JList[TileSet]]
            bgs.setModel(backgrounds)
            
            bgs.setVisibleRowCount(4)
            
            val addBg = engine.find("add-bg-button").asInstanceOf[JButton]
            addBg.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    val tileset = new TileSet("New Background TileSet")
                    backgrounds.add(tileset)
                    tileset.editDialog(view, viewScroll, bgs, animations.getAll())
                } 
            })
            
            val editBg = engine.find("edit-bg-button").asInstanceOf[JButton]
            editBg.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    if (backgrounds.getSize() > 0 && bgs.getSelectedIndex() != -1){
                        backgrounds.getElementAt(bgs.getSelectedIndex()).editDialog(view, viewScroll, bgs, animations.getAll())
                    }
                } 
            })
            
            val removeBg = engine.find("remove-bg-button").asInstanceOf[JButton]
            removeBg.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    if (backgrounds.getSize() > 0 && bgs.getSelectedIndex() != -1){
                        backgrounds.remove(bgs.getSelectedIndex())
                        view.revalidate()
                        viewScroll.repaint()
                    }
                } 
            })
            
            val fgs = engine.find("foregrounds").asInstanceOf[JList[TileSet]]
            fgs.setModel(foregrounds)
            
            fgs.setVisibleRowCount(4)
            
            val addFg = engine.find("add-fg-button").asInstanceOf[JButton]
            addFg.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    val tileset = new TileSet("New Foreground TileSet")
                    foregrounds.add(tileset)
                    tileset.editDialog(view, viewScroll, fgs, animations.getAll())
                } 
            })
            
            val editFg = engine.find("edit-fg-button").asInstanceOf[JButton]
            editFg.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    if (foregrounds.getSize() > 0 && fgs.getSelectedIndex() != -1){
                        foregrounds.getElementAt(fgs.getSelectedIndex()).editDialog(view, viewScroll, fgs, animations.getAll())
                    }
                } 
            })
            
            val removeFg = engine.find("remove-fg-button").asInstanceOf[JButton]
            removeFg.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    if (foregrounds.getSize() > 0 && fgs.getSelectedIndex() != -1){
                        foregrounds.remove(fgs.getSelectedIndex())
                        view.revalidate()
                        viewScroll.repaint()
                    }
                } 
            })
            
            val currentBg = engine.find("current-bg-button").asInstanceOf[JButton]
            currentBg.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    if (backgrounds.getSize() > 0 && bgs.getSelectedIndex() != -1){
                        clearCurrentTileSet()
                        backgrounds.getElementAt(bgs.getSelectedIndex()).isCurrent = true
                        view.revalidate()
                        viewScroll.repaint()
                        fgs.repaint()
                        bgs.repaint()
                    }
                } 
            })
            
            val currentFg = engine.find("current-fg-button").asInstanceOf[JButton]
            currentFg.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    if (foregrounds.getSize() > 0 && fgs.getSelectedIndex() != -1){
                        clearCurrentTileSet()
                        foregrounds.getElementAt(fgs.getSelectedIndex()).isCurrent = true
                        view.revalidate()
                        viewScroll.repaint()
                        fgs.repaint()
                        bgs.repaint()
                    }
                } 
            })
        }
        
        
        // Collisions
        {
            val collisions = engine.find("collision-maps").asInstanceOf[JList[Area]]
            collisions.setModel(collisionMaps)
            
            collisions.setVisibleRowCount(4)
            
            val add = engine.find("add-collision-button").asInstanceOf[JButton]
            add.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    val area = new Area("New Area")
                    collisionMaps.add(area)
                    area.editDialog(view, viewScroll, collisions)
                } 
            })
            
            val edit = engine.find("edit-collision-button").asInstanceOf[JButton]
            edit.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    if (collisionMaps.getSize() > 0 && collisions.getSelectedIndex() != -1){
                        collisionMaps.getElementAt(collisions.getSelectedIndex()).editDialog(view, viewScroll, collisions)
                    }
                } 
            })
            
            val remove = engine.find("remove-collision-button").asInstanceOf[JButton]
            remove.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    if (collisionMaps.getSize() > 0 && collisions.getSelectedIndex() != -1){
                        collisionMaps.remove(collisions.getSelectedIndex())
                        view.revalidate()
                        viewScroll.repaint()
                    }
                } 
            })
        }
        
        pane
    }
    
    def connectScaleOffset(engine:SwingEngine, view:JPanel, viewScroll:JScrollPane){
        val scroll = engine.find( "scale" ).asInstanceOf[JSlider]
        val scaleLabel = engine.find( "scale-label" ).asInstanceOf[JLabel]
        scroll.addChangeListener( new ChangeListener(){
            override def stateChanged(event:ChangeEvent){
                scale = scroll.getValue().toDouble * 2.0 / scroll.getMaximum()
                scaleLabel.setText("Scale: " + scale)
                view.revalidate()
                viewScroll.repaint()
            }
        })
        
        var offset = engine.find("offset-x").asInstanceOf[JSpinner]
        var model = new SpinnerNumberModel()
        model.setValue(offsetX)
        offset.setModel(model)
        offset.addChangeListener(new ChangeListener(){
            override def stateChanged(event:ChangeEvent){
                val spinner = event.getSource().asInstanceOf[JSpinner]
                val i = spinner.getValue().asInstanceOf[java.lang.Integer]
                offsetX = i.intValue()
                view.revalidate()
                viewScroll.repaint()
            }
        })
        
        offset = engine.find("offset-y").asInstanceOf[JSpinner]
        model = new SpinnerNumberModel()
        model.setValue(offsetY)
        offset.setModel(model)
        offset.addChangeListener(new ChangeListener(){
            override def stateChanged(event:ChangeEvent){
                val spinner = event.getSource().asInstanceOf[JSpinner]
                val i = spinner.getValue().asInstanceOf[java.lang.Integer]
                offsetY = i.intValue()
                view.revalidate()
                viewScroll.repaint()
            }
        })
    }
    
    def createUpdateTimer(view:JPanel, viewScroll:JScrollPane) = {
        actions = new AnimationUpdater(view, viewScroll, animations)
    }
}
