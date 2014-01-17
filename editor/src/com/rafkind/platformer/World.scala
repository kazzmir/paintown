package com.rafkind.platformer

import java.awt._
import java.awt.image._
import javax.swing._
import java.io._
import javax.imageio._
import java.awt.event._
import javax.swing.event._
import org.swixml.SwingEngine
import javax.swing.filechooser.FileFilter

import scala.collection.immutable.List

import com.rafkind.paintown.exception.LoadException
import com.rafkind.paintown.TokenReader
import com.rafkind.paintown.Token
import com.rafkind.paintown.MaskedImage

class AnimationUpdater(val view:JPanel, val viewScroll:JScrollPane, val animations:AnimationListModel) extends Thread {
    var fps = 60
    var frameCount = 0
    var done = false
    
    override def run() = {
        val GAME_HERTZ:Double = 30.0
        val TIME_BETWEEN_UPDATES:Double = 1000000000 / GAME_HERTZ
        val MAX_UPDATES_BEFORE_RENDER = 5
        val TARGET_FPS:Double = 60
        val TARGET_TIME_BETWEEN_RENDERS:Double = 1000000000 / TARGET_FPS
        
        var lastUpdateTime:Double = System.nanoTime()
        var lastRenderTime:Double = System.nanoTime()
    
        //Simple way of finding FPS.
        var lastSecondTime = (lastUpdateTime / 1000000000).intValue
    
        while (!done){
            var now:Double = System.nanoTime()
            var updateCount = 0
            
            while( now - lastUpdateTime > TIME_BETWEEN_UPDATES && updateCount < MAX_UPDATES_BEFORE_RENDER ){
                update()
                lastUpdateTime = lastUpdateTime + TIME_BETWEEN_UPDATES
                updateCount = updateCount + 1
            }

            if ( now - lastUpdateTime > TIME_BETWEEN_UPDATES){
                lastUpdateTime = now - TIME_BETWEEN_UPDATES
            }
            
            //Render. 
            draw()
            lastRenderTime = now
            
            //Update the frames we got.
            var thisSecond = (lastUpdateTime / 1000000000).intValue
            if (thisSecond > lastSecondTime){
                //System.out.println("NEW SECOND " + thisSecond + " " + frameCount)
                fps = frameCount
                frameCount = 0
                lastSecondTime = thisSecond
            }
    
            //Yield until it has been at least the target time between renders. This saves the CPU from hogging.
            while ( now - lastRenderTime < TARGET_TIME_BETWEEN_RENDERS && now - lastUpdateTime < TIME_BETWEEN_UPDATES){
                Thread.`yield`()
                try {
                    Thread.sleep(1)
                } catch {
                    case ex:Exception => {} 
                }
                now = System.nanoTime()
            }
        }
    }
    
    def update() = {
        animations.getAll().foreach{
            case (animation) => animation.update()
        }
    }
    
    def draw() = {
        view.revalidate()
        viewScroll.repaint()
        frameCount = frameCount + 1
    }
    
    def terminate() = {
        this.synchronized{
            done = true
        }
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
    
    //! Fill color
    var fillColor:Color = new Color(204,204,204)
    var fillColorEnabled = true
    
    //! Overlay
    var overlay:ImageHolder = null
    var overlayAlpha:Float = 0.5f
    
    //! Animations
    val animations = new AnimationListModel()
    
    //! Background Tilesets
    val backgrounds = new TileSetListModel()
    
    //! Foreground Tilesets
    var foregrounds = new TileSetListModel()
    
    //! Collision maps
    var collisionMaps = new CollisionMap()
    
    //! Script data paths
    val scriptPaths = new ScriptPathData()
    
    //! Script objects
    var scriptObjects = new ScriptObjectData()
    
    //! Scripts
    var worldScripts = new ScriptData()
    
    //! Collect collision info
    var inputCollision:Boolean = false
    
    var collisionX:Int = 0
    var collisionY:Int = 0
    var collisionWidth:Int = 0
    var collisionHeight:Int = 0
    
    //! Copy object to
    var scriptObjectCopy:Boolean = false
    
    //! Grab object
    var scriptObjectGrabbed:Boolean = false
    var scriptObjectOffsetX:Int = 0
    var scriptObjectOffsetY:Int = 0
    
    //! actions (updating animations)
    var actions:AnimationUpdater = null
    
    //! Enable animations
    var enableAnimations:Boolean = false
    
    //! show fps
    var displayFps:Boolean = false
    
    //! Panel access
    var worldValuesEngine:SwingEngine = null
    var tilesetsEngine:SwingEngine = null
    var collisionsEngine:SwingEngine = null
    var scriptsEngine:SwingEngine = null
    var animationsEngine:SwingEngine = null
    
    if (_path != null){
        load(_path)
    } else {
        val tileset = new TileSet("Empty tileset", width, height)
        tileset.isCurrent = true
        backgrounds.add(tileset)
    }
    
    def render(g:Graphics2D, x:Int, y:Int) = {
        g.scale(scale, scale)
        
        // Entire map
        if (fillColorEnabled){
            g.setColor( fillColor )
            g.fillRect(offsetX, offsetY, this.width, this.height)
        }
        
        // backgrounds
        backgrounds.getAll().foreach{
            case (tileset) => tileset.render(g, offsetX, offsetY, animations)
        }
        
        // Objects
        scriptObjects.render(g, offsetX, offsetY)
        
        // foregrounds
        foregrounds.getAll().foreach{
            case (tileset) => tileset.render(g, offsetX, offsetY, animations)
        }
        
        // Collision maps
        collisionMaps.render(g, offsetX, offsetY)
        
        if (inputCollision){
            g.setColor(new Color(0, 255, 0))
            g.drawRect(offsetX + collisionX, offsetY + collisionY, collisionWidth, collisionHeight)
        }
        
        // Draw overlay
        if (overlay != null){
            val old:Composite = g.getComposite()
            g.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, overlayAlpha))
            overlay.render(g, offsetX, offsetY)
            g.setComposite(old)
        }
        
        // Viewport resolution
        g.setColor(new Color(0, 0, 255,40))
        g.drawRect(offsetX, offsetY, this.resolutionX, this.resolutionY)
        
        // FPS
        if (displayFps){
            g.setColor(new Color(0,128, 0))
            val placement = 5 * (5/scale).intValue
            g.setFont(new Font("Arial", placement, placement))
            g.drawString("FPS: " + actions.fps, (x/scale).intValue + placement, (y/scale).intValue + placement)
        }
    }
    
    def addAnimation(animation:Animation) = {
        animations.add(animation)
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
        
        val colorToken = head.findToken("fill-color")
        if (colorToken != null){
            fillColor = new Color(colorToken.readInt(0), colorToken.readInt(1), colorToken.readInt(2))
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
                val tileset = new TileSet("Not assigned", width, height)
                tileset.readToken(backgroundIterator.next(), animations)
                backgrounds.add(tileset)
            } catch {
                case ex:LoadException => { println(ex.getMessage()) }
            }
        }
        
        val foregroundIterator = head.findTokens("foreground").iterator()
        while(foregroundIterator.hasNext()){
            try {
                val tileset = new TileSet("Not assigned", width, height)
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
        
        scriptPaths.readToken(head)
        
        worldScripts.readToken(head)
        
        scriptObjects.readToken(head, animations)

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
        dimension.setSize((width * scale) + (offsetX * 2 * scale), (height * scale) + (offsetY * 2 * scale))
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
        mechanics.addToken(Array("acceleration", String.valueOf(acceleration)))
        world.addToken(mechanics)
        world.addToken(Array("fill-color", String.valueOf(fillColor.getRed()),String.valueOf(fillColor.getGreen()),String.valueOf(fillColor.getBlue())))
        /*
            FIXME 
            Temporarily add a camera
            need to create a camera construction tool
        */
        val camera = new Token()
        camera.addToken(new Token(camera, "camera"))
        camera.addToken(Array("id", "0"))
        camera.addToken(Array("dimensions", String.valueOf(width), String.valueOf(height)))
        camera.addToken(Array("start", "0", "0"))
        camera.addToken(Array("viewport", "0", "0", String.valueOf(resolutionX), String.valueOf(resolutionY)))
        camera.addToken(Array("speed", "1.5"))
        camera.addToken(Array("velocity", ".5"))
        camera.addToken(Array("follow-variance", "2"))
        camera.addToken(Array("smooth-scrolling", "1"))
        camera.addToken(Array("smooth-scroll-modifier", "0"))
        world.addToken(camera)
        
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
        
        // Script paths
        scriptPaths.getAll().foreach{
            case (path) => world.addToken(path.toToken())
        }
        
        // Scripts
        worldScripts.getAll().foreach{
            case (script) => world.addToken(script.toToken())
        }
        
        // Script objects
        scriptObjects.getAll().foreach{
            case (script) => world.addToken(script.toToken())
        }

        world
    }
    
    def leftClick(x:Int, y:Int) = {
        if (inputCollision){
            collisionX = x
            collisionY = y
        } else if (scriptObjectCopy){
            // Do nothing
        } else if (getCurrentTileSet() != null && getCurrentAnimation() != null){
            addTile(x,y)
        } else {
            selectItem(x,y)
        }
    }
    
    def leftRelease(x:Int, y:Int) = {
        if (inputCollision){
            val w= x - collisionX
            val h = y - collisionY
            if (w >= 0 && h >= 0){
                addArea(collisionX, collisionY, w, h)
            }
            inputCollision = false
            collisionX = 0
            collisionY = 0
            collisionWidth = 0
            collisionHeight = 0
        } else if (scriptObjectCopy){
            copyObject(x,y)
            scriptObjectCopy = false;
        } else {
            scriptObjectGrabbed = false;
            scriptObjectOffsetX = 0
            scriptObjectOffsetY = 0
        }
    }
    
    def rightClick(x:Int, y:Int) = {
        if (inputCollision){
        } else if (getCurrentTileSet() != null){
            removeTile(x,y)
        }
    }
    
    def rightRelease(x:Int, y:Int) = {
        if (inputCollision){
        }
    }
    
    def leftDrag(x:Int, y:Int) = {
        if (scriptObjectGrabbed){
            moveObject(x,y)
        } else if (!inputCollision){
            if (getCurrentTileSet() != null && getCurrentAnimation() != null){
                addTile(x,y)
            }
        } else if (inputCollision){
            collisionWidth = x - collisionX
            collisionHeight = y - collisionY
        }
    }
    
    def rightDrag(x:Int, y:Int) = {
        if (inputCollision){
        } else if (getCurrentTileSet() != null){
            removeTile(x,y)
        }
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
    
    def addArea(x:Int, y:Int, w:Int, h:Int) = {
        val area = new Area("New collision map")
        area.x = x
        area.y = y
        area.width = w
        area.height = h
        val areaName = JOptionPane.showInputDialog(null, "Set name of collision map", "New Collision Map", 1)
        if (areaName != null){
            area.name = areaName
            collisionMaps.add(area)
        }
    }
    
    def copyObject(x:Int, y:Int) = {
        val objects = scriptsEngine.find("script-objects").asInstanceOf[JList[Area]]
        if (objects.getSelectedIndex() != -1){
            val script = new ScriptObject("New script object")
            script.readToken(scriptObjects.getElementAt(objects.getSelectedIndex()).toToken(), animations)
            script.x = x
            script.y = y
            scriptObjects.add(script)
            //objects.ensureIndexIsVisible(objects.getModel().indexOf(script))
        }
    }
    
    def moveObject(x:Int, y:Int) = {
        val objects = scriptsEngine.find("script-objects").asInstanceOf[JList[Area]]
        if (objects.getSelectedIndex() != -1){
            val script = scriptObjects.getElementAt(objects.getSelectedIndex())
            script.x = x + scriptObjectOffsetX
            script.y = y + scriptObjectOffsetY
        }
    }
    
    def collides(checkX:Int, checkY:Int, x:Int, y:Int, width:Int, height:Int) = {
        !((checkX > (x + width)) || (checkY > (y+height)) || (x > checkX) || (y > checkY))
    }
    
    def selectItem(x:Int, y:Int) = {
        // Select collision map or object will select both if it's in the same region
        val collisions = collisionsEngine.find("collision-maps").asInstanceOf[JList[Area]]
        collisionMaps.getAll().zipWithIndex.foreach{
            case (area, index) => {
                if (collides(x, y, area.x, area.y, area.width, area.height)){
                    collisions.setSelectedIndex(index)
                    collisions.ensureIndexIsVisible(index)
                }
            }
        }
        val objects = scriptsEngine.find("script-objects").asInstanceOf[JList[Area]]
        scriptObjects.getAll().zipWithIndex.foreach{
            case (script, index) => {
                if (collides(x, y, script.x, script.y, script.width, script.height)){
                    objects.setSelectedIndex(index)
                    objects.ensureIndexIsVisible(index)
                    scriptObjectGrabbed = true;
                    scriptObjectOffsetX = x - (script.x + script.width)
                    scriptObjectOffsetY = y - (script.y + script.height)
                }
            }
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
        worldValuesEngine = engine
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
        
        // Fill color
        {
            val viewColor = new JPanel(){
                override def getPreferredSize():Dimension = {
                    new Dimension(25,25)
                }

                override def paintComponent(g:Graphics){
                    g.setColor(fillColor)
                    g.fillRect(0, 0, this.getWidth(), this.getHeight())
                    g.setColor(Color.BLACK)
                    g.drawRect(0,0,this.getWidth()-1,this.getHeight()-1)
                }
            }
            val colorPanel = engine.find("fill-color-display").asInstanceOf[JPanel]
            colorPanel.add(viewColor)
            viewColor.revalidate()
            colorPanel.repaint()
            
            val button = engine.find("fill-color").asInstanceOf[JButton]
            button.addActionListener(new ActionListener() {
                def actionPerformed(e:ActionEvent) = {
                    val color = JColorChooser.showDialog(button, "Select a Fill Color", fillColor);
                    if (color != null){
                        fillColor = color
                        view.revalidate()
                        viewScroll.repaint()
                        viewColor.revalidate()
                        colorPanel.repaint()
                    }
                }
            })
            
            val checkbox = engine.find("enable-fill-color").asInstanceOf[JCheckBox]
            checkbox.setSelected(fillColorEnabled)
            checkbox.addActionListener(new ActionListener() {
                def actionPerformed(e:ActionEvent) = {
                    fillColorEnabled = checkbox.isSelected()
                    view.revalidate()
                    viewScroll.repaint()
                }
            })
        }
        
        // Overlay
        {
            var slider = engine.find("overlay-slider").asInstanceOf[JSlider]
            slider.setValue((50*overlayAlpha).toInt)
            slider.addChangeListener( new ChangeListener(){
                override def stateChanged(event:ChangeEvent){
                    overlayAlpha = (slider.getValue().toInt * .02).toFloat
                    view.revalidate()
                    viewScroll.repaint()
                }
            })
            
            var overlayField = engine.find("overlay").asInstanceOf[JTextField]
            overlayField.setEditable(false)
            if (overlay != null){
                overlayField.setText(overlay.toString())
            }
            val set = engine.find("overlay-button").asInstanceOf[JButton]
            set.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    val chooser = new JFileChooser(MapEditor.getDataPath("/"))
                    chooser.setFileFilter(new FileFilter(){
                        def accept(f:File):Boolean = {
                            f.isDirectory() || f.getName().endsWith( ".png" )
                        }

                        def getDescription():String = {
                            "Png files"
                        }
                    })
                    val returnVal = chooser.showOpenDialog(pane)
                    if (returnVal == JFileChooser.APPROVE_OPTION){
                        val choosen:File = chooser.getSelectedFile()
                        val base = MapEditor.getDataPath("/").getPath()
                        val absolute = choosen.getPath()
                        val relative = absolute.replace(base,"")
                        overlay = new ImageHolder(new File("."), new File(relative))
                        overlayField.setText(overlay.toString())
                        view.revalidate()
                        viewScroll.repaint()
                    }
                } 
            })
        }
        
        // Scroll
        {
            val scroll = engine.find( "scale" ).asInstanceOf[JSlider]
            val scaleLabel = engine.find( "scale-label" ).asInstanceOf[JLabel]
            scroll.setValue((scroll.getMaximum() * (scale/4.0)).toInt)
            scroll.addChangeListener( new ChangeListener(){
                override def stateChanged(event:ChangeEvent){
                    scale = scroll.getValue().toDouble * 4.0 / scroll.getMaximum()
                    scaleLabel.setText("Scale: " + scale)
                    view.revalidate()
                    viewScroll.repaint()
                }
            })
        }
        
        // offsets
        {
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
        
        // fps
        {
            val fps = engine.find("display-fps").asInstanceOf[JCheckBox]
            fps.setSelected(displayFps)
            fps.setEnabled(false)
            fps.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    displayFps = fps.isSelected()
                } 
            })
            
            val anims = engine.find("enable-animations").asInstanceOf[JCheckBox]
            anims.setSelected(enableAnimations)
            anims.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    enableAnimations = anims.isSelected()
                    if (enableAnimations){
                        if (!fps.isEnabled()){
                            fps.setEnabled(true)
                        }
                        if (actions == null){
                            start(view, viewScroll)
                        }
                    } else {
                        displayFps = false
                        fps.setEnabled(false)
                        fps.setSelected(false)
                        stop()
                    }
                } 
            })
        }
        
        pane
    }
    
    
    def createTilesetsPanel(view:JPanel, viewScroll:JScrollPane, tabbed:JTabbedPane, mainEngine:SwingEngine):JPanel = {
        val engine = new SwingEngine( "platformer/world-tilesets.xml" )
        tilesetsEngine = engine
        val pane = engine.getRootComponent().asInstanceOf[JPanel]
        
        val currentTileset = mainEngine.find("current-tileset").asInstanceOf[JLabel]
        val bgs = engine.find("backgrounds").asInstanceOf[JList[TileSet]]
        val fgs = engine.find("foregrounds").asInstanceOf[JList[TileSet]]
        
        // Backgrounds and foregrounds
        {
            bgs.setModel(backgrounds)
            
            bgs.setVisibleRowCount(4)
            
            val addBg = engine.find("add-bg-button").asInstanceOf[JButton]
            addBg.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    val tileset = new TileSet("New Background TileSet", width, height)
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
            
            val upBg = engine.find("up-bg-button").asInstanceOf[JButton]
            upBg.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    if (backgrounds.getSize() > 0 && bgs.getSelectedIndex() != -1){
                        var index = bgs.getSelectedIndex()
                        backgrounds.swap(index, index-1)
                        index = index-1
                        bgs.setSelectedIndex(index)
                        bgs.updateUI()
                    }
                } 
            })
            
            val downBg = engine.find("down-bg-button").asInstanceOf[JButton]
            downBg.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    if (backgrounds.getSize() > 0 && bgs.getSelectedIndex() != -1){
                        var index = bgs.getSelectedIndex()
                        backgrounds.swap(index, index+1)
                        index = index+1
                        bgs.setSelectedIndex(index)
                        bgs.updateUI()
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
            
            fgs.setModel(foregrounds)
            
            fgs.setVisibleRowCount(4)
            
            val addFg = engine.find("add-fg-button").asInstanceOf[JButton]
            addFg.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    val tileset = new TileSet("New Foreground TileSet", width, height)
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
            
            val upFg = engine.find("up-fg-button").asInstanceOf[JButton]
            upFg.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    if (foregrounds.getSize() > 0 && fgs.getSelectedIndex() != -1){
                        var index = fgs.getSelectedIndex()
                        foregrounds.swap(index, index-1)
                        index = index-1
                        fgs.setSelectedIndex(index)
                        fgs.updateUI()
                    }
                } 
            })
            
            val downFg = engine.find("down-fg-button").asInstanceOf[JButton]
            downFg.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    if (foregrounds.getSize() > 0 && fgs.getSelectedIndex() != -1){
                        var index = fgs.getSelectedIndex()
                        foregrounds.swap(index, index+1)
                        index = index+1
                        fgs.setSelectedIndex(index)
                        fgs.updateUI()
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
                        if (backgrounds.getElementAt(bgs.getSelectedIndex()).isCurrent){
                            backgrounds.getElementAt(bgs.getSelectedIndex()).isCurrent = false
                            view.revalidate()
                            viewScroll.repaint()
                            fgs.repaint()
                            bgs.repaint()
                            currentTileset.setText("None")
                        } else {
                            clearCurrentTileSet()
                            val background = backgrounds.getElementAt(bgs.getSelectedIndex())
                            background.isCurrent = true
                            view.revalidate()
                            viewScroll.repaint()
                            fgs.repaint()
                            bgs.repaint()
                            currentTileset.setText(background.name)
                        }
                    }
                } 
            })
            
            val currentFg = engine.find("current-fg-button").asInstanceOf[JButton]
            currentFg.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    if (foregrounds.getSize() > 0 && fgs.getSelectedIndex() != -1){
                        if (foregrounds.getElementAt(fgs.getSelectedIndex()).isCurrent){
                            foregrounds.getElementAt(fgs.getSelectedIndex()).isCurrent = false
                            view.revalidate()
                            viewScroll.repaint()
                            fgs.repaint()
                            bgs.repaint()
                            currentTileset.setText("None")
                        } else {
                            clearCurrentTileSet()
                            val foreground = foregrounds.getElementAt(fgs.getSelectedIndex())
                            foreground.isCurrent = true
                            view.revalidate()
                            viewScroll.repaint()
                            fgs.repaint()
                            bgs.repaint()
                            currentTileset.setText(foreground.name)
                        }
                    }
                } 
            })
            
            val clear = mainEngine.find("clear-selections").asInstanceOf[JButton]
            clear.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    clearCurrentAnimation()
                    clearCurrentTileSet()
                    mainEngine.find("current-animation").asInstanceOf[JLabel].setText("None")
                    currentTileset.setText("None")
                    //anims.repaint()
                    animationsEngine.find("anims").asInstanceOf[JList[Animation]].repaint()
                    fgs.repaint()
                    bgs.repaint()
                    view.revalidate()
                    viewScroll.repaint()
                } 
            })
        }
        
        pane
    }
    
    
    def createCollisionsPanel(view:JPanel, viewScroll:JScrollPane, tabbed:JTabbedPane):JPanel = {
        val engine = new SwingEngine( "platformer/world-collisions.xml" )
        collisionsEngine = engine
        val pane = engine.getRootComponent().asInstanceOf[JPanel]
        
        // Collisions
        {
            val collisions = engine.find("collision-maps").asInstanceOf[JList[Area]]
            collisions.setModel(collisionMaps)
            
            collisions.setVisibleRowCount(4)
            
            val grab = engine.find("grab-collision-button").asInstanceOf[JButton]
            grab.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    scriptObjectCopy = false
                    inputCollision = true
                } 
            })
            
            val add = engine.find("add-collision-button").asInstanceOf[JButton]
            add.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    val area = new Area("New collision map")
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
    
    
    def createScriptsPanel(view:JPanel, viewScroll:JScrollPane, tabbed:JTabbedPane):JPanel = {
        val engine = new SwingEngine( "platformer/world-scripts.xml" )
        scriptsEngine = engine
        val pane = engine.getRootComponent().asInstanceOf[JPanel]
        
        // Script paths
        {
            val paths = engine.find("script-data-paths").asInstanceOf[JList[ScriptPath]]
            paths.setModel(scriptPaths)
            
            paths.setVisibleRowCount(4)
            
            val add = engine.find("add-path-button").asInstanceOf[JButton]
            add.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    val path = new ScriptPath("newpath")
                    if (path.editDialog(view)){
                        scriptPaths.add(path)
                    }
                } 
            })
            
            val remove = engine.find("remove-path-button").asInstanceOf[JButton]
            remove.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    if (scriptPaths.getSize() > 0 && paths.getSelectedIndex() != -1){
                        scriptPaths.remove(paths.getSelectedIndex())
                    }
                } 
            })
        }
        
        // Scripts
        {
            val scripts = engine.find("scripts").asInstanceOf[JList[Script]]
            scripts.setModel(worldScripts)
            
            scripts.setVisibleRowCount(4)
            
            val add = engine.find("add-script-button").asInstanceOf[JButton]
            add.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    val script = new Script("New script")
                    worldScripts.add(script)
                    script.editDialog(scripts)
                } 
            })
            
            val edit = engine.find("edit-script-button").asInstanceOf[JButton]
            edit.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    if (worldScripts.getSize() > 0 && scripts.getSelectedIndex() != -1){
                        worldScripts.getElementAt(scripts.getSelectedIndex()).editDialog(scripts)
                    }
                } 
            })
            
            val remove = engine.find("remove-script-button").asInstanceOf[JButton]
            remove.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    if (worldScripts.getSize() > 0 && scripts.getSelectedIndex() != -1){
                        worldScripts.remove(scripts.getSelectedIndex())
                    }
                } 
            })
        }
        
        // Script objects
        {
            val scripts = engine.find("script-objects").asInstanceOf[JList[ScriptObject]]
            scripts.setModel(scriptObjects)
            
            scripts.setVisibleRowCount(4)
            
            val add = engine.find("add-script-object-button").asInstanceOf[JButton]
            add.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    val script = new ScriptObject("New script object")
                    scriptObjects.add(script)
                    script.editDialog(view, viewScroll, scripts, animations.getAll())
                } 
            })
            
            val edit = engine.find("edit-script-object-button").asInstanceOf[JButton]
            edit.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    if (scriptObjects.getSize() > 0 && scripts.getSelectedIndex() != -1){
                        scriptObjects.getElementAt(scripts.getSelectedIndex()).editDialog(view, viewScroll, scripts, animations.getAll())
                    }
                } 
            })
            
            val copy = engine.find("copy-script-object-button").asInstanceOf[JButton]
            copy.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    if (scriptObjects.getSize() > 0 && scripts.getSelectedIndex() != -1){
                        val script = new ScriptObject("new")
                        script.readToken(scriptObjects.getElementAt(scripts.getSelectedIndex()).toToken(), animations)
                        scriptObjects.add(script)
                        script.editDialog(view, viewScroll, scripts, animations.getAll())
                    }
                } 
            })
            
            val copyTo = engine.find("copy-script-object-to-button").asInstanceOf[JButton]
            copyTo.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    if (scriptObjects.getSize() > 0 && scripts.getSelectedIndex() != -1){
                        inputCollision = false
                        scriptObjectCopy = true
                    }
                } 
            })
            
            val remove = engine.find("remove-script-object-button").asInstanceOf[JButton]
            remove.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    if (scriptObjects.getSize() > 0 && scripts.getSelectedIndex() != -1){
                        scriptObjects.remove(scripts.getSelectedIndex())
                        view.revalidate()
                        viewScroll.repaint()
                    }
                } 
            })
        }
        
        pane
    }
    
    // Animations pane
    def createAnimationsPanel(view:JPanel, viewScroll:JScrollPane, tabbed:JTabbedPane, mainEngine:SwingEngine):JPanel = {
        val engine = new SwingEngine( "platformer/world-animations.xml" )
        animationsEngine = engine
        val pane = engine.getRootComponent().asInstanceOf[JPanel]
        
        // Animations
        {
            val currentAnimation = mainEngine.find("current-animation").asInstanceOf[JLabel]
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
            
            val copy = engine.find("copy-anim-button").asInstanceOf[JButton]
            copy.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    if (animations.getSize() > 0 && anims.getSelectedIndex() != -1){
                        val animation = new Animation("new")
                        animation.readToken(animations.getElementAt(anims.getSelectedIndex()).toToken())
                        animation.name = animation.name + " (Copy)"
                        addAnimation(animation)
                        animation.editDialog(view, viewScroll, anims)
                    }
                } 
            })
            
            val current = engine.find("current-anim-button").asInstanceOf[JButton]
            current.addActionListener(new ActionListener() { 
                def actionPerformed(e:ActionEvent) = {
                    if (animations.getSize() > 0 && anims.getSelectedIndex() != -1){
                        if (animations.getElementAt(anims.getSelectedIndex()).isCurrent){
                            animations.getElementAt(anims.getSelectedIndex()).isCurrent = false
                            view.revalidate()
                            viewScroll.repaint()
                            anims.repaint()
                            currentAnimation.setText("None")
                        } else {
                            clearCurrentAnimation()
                            val animation = animations.getElementAt(anims.getSelectedIndex())
                            animation.isCurrent = true
                            view.revalidate()
                            viewScroll.repaint()
                            anims.repaint()
                            currentAnimation.setText(animation.name)
                        }
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
        
        pane
    }
    
    def start(view:JPanel, viewScroll:JScrollPane) = {
        actions = new AnimationUpdater(view, viewScroll, animations)
        actions.start()
    }
    
    def stop() = {
        if (actions != null){
            actions.terminate()
            actions = null
        }
    }
}
