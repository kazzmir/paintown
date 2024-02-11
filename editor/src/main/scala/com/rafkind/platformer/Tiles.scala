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

class Tile{
    var animation:Animation = null
    
    var row:Int = 0
    var column:Int = 0
    
    def readToken(token:Token, animations:AnimationListModel) = {
        if (!token.getName().equals("tile")){
            throw new LoadException( "Starting token is not 'tile'" )
        }
        val name = token.findToken("animation")
        if (name != null){
            animation = animations.get(name.readString(0))
        }
        val position = token.findToken("position")
        if (position != null){
            row = position.readInt(0)
            column = position.readInt(1)
        }
    }
    
    def toToken():Token = {
        val tile = new Token()
        if (animation != null){
            tile.addToken(new Token(tile, "tile"))
            tile.addToken(Array("animation", String.valueOf(animation.name)))
            tile.addToken(Array("position", String.valueOf(row), String.valueOf(column)))
        }
        tile
    }
    
    override def toString:String = {
        toToken().toString()
    }
    
    def editDialog(view:JPanel, viewScroll:JScrollPane, list:JList[Tile], animations:List[Animation]) = {
        try {
            val engine = new SwingEngine("platformer/tile.xml")
            val pane = engine.find("dialog").asInstanceOf[JDialog]
            
            {
                val animationField = engine.find("animation").asInstanceOf[JComboBox[Animation]]
                animationField.setModel(new DefaultComboBoxModel(animations.toArray))
                
                if (animation != null){
                    animationField.setSelectedIndex(animations.indexOf(animation))
                } else {
                    animationField.setSelectedIndex(0)
                    animation = animations(0)
                }
                
                val animationScroll = engine.find("animation-view").asInstanceOf[JPanel]
                val animationView = new JPanel(){
                    override def getPreferredSize():Dimension = {
                        new Dimension(150, 150)
                    }

                    override def paintComponent(g:Graphics){
                        val h = viewScroll.getHorizontalScrollBar()
                        val v = viewScroll.getVerticalScrollBar()
                        g.setColor(new Color(64, 64, 64))
                        g.fillRect(0, 0, this.getWidth(), this.getHeight())
                        if (animationField.getSelectedIndex() != -1){
                            var animation = animations(animationField.getSelectedIndex())
                            animation.renderScaled(g.asInstanceOf[Graphics2D], 50, 50, 49, 49)
                        }
                    }
                }
                
                animationField.addActionListener(new ActionListener() { 
                    def actionPerformed(e:ActionEvent) = {
                        animation = animations(animationField.getSelectedIndex())
                        list.updateUI()
                        list.revalidate()
                        list.repaint()
                        animationView.revalidate()
                        animationView.repaint()
                    } 
                })
                
                animationScroll.add(animationView)
                animationView.revalidate()
            }
            
            {
                val rowField = engine.find("row").asInstanceOf[JSpinner]
                val model = new SpinnerNumberModel()
                rowField.setModel(model)
                model.setValue(row)
                rowField.addChangeListener(new ChangeListener(){
                    override def stateChanged(event:ChangeEvent){
                        val spinner = event.getSource().asInstanceOf[JSpinner]
                        val i = spinner.getValue().asInstanceOf[java.lang.Integer]
                        row = i.intValue()
                        list.updateUI()
                        list.revalidate()
                        list.repaint()
                        view.revalidate()
                        viewScroll.repaint()
                    }
                })
            }
            
            {
                val columnField = engine.find("column").asInstanceOf[JSpinner]
                val model = new SpinnerNumberModel()
                columnField.setModel(model)
                model.setValue(column)
                columnField.addChangeListener(new ChangeListener(){
                    override def stateChanged(event:ChangeEvent){
                        val spinner = event.getSource().asInstanceOf[JSpinner]
                        val i = spinner.getValue().asInstanceOf[java.lang.Integer]
                        column = i.intValue()
                        list.updateUI()
                        list.revalidate()
                        list.repaint()
                        view.revalidate()
                        viewScroll.repaint()
                    }
                })
            }
            
            // Close
            {
                val close = engine.find("close").asInstanceOf[JButton]
                close.addActionListener(new ActionListener() { 
                    def actionPerformed(e:ActionEvent) = {
                        pane.setVisible(false)
                        view.revalidate()
                        viewScroll.repaint()
                        list.updateUI()
                        list.revalidate()
                        list.repaint()
                    } 
                })
            }
            
            // Show Dialog
            pane.repaint()
            pane.setModal(true)
            pane.setVisible(true)
        } catch {
            case e:Exception => JOptionPane.showMessageDialog(null, "error on opening, reason: " + e.getMessage(), "ERROR", JOptionPane.ERROR_MESSAGE)
        }
    }
}

class TileListModel extends ListModel[Tile] {
    var data:List[Tile] = List[Tile]()
    var listeners = List[ListDataListener]()

    def add(tile:Tile){
        var updated = false
        for((t,index) <- data.view.zipWithIndex){
            if (t.row == tile.row && t.column == tile.column){
                updated = true
                data = data.updated(index, tile)
                val event = new ListDataEvent(this, ListDataEvent.CONTENTS_CHANGED, index, index)
                for (listener <- listeners){
                    listener.intervalAdded(event)
                }
            }
        }
        if (!updated){
            data = data :+ tile
            val event = new ListDataEvent(this, ListDataEvent.INTERVAL_ADDED, data.size, data.size)
            for (listener <- listeners){
                listener.intervalAdded(event)
            }
        }
    }
    
    def remove(index:Int){
        data = data.filterNot(data.indexOf(_) == index)
        val event = new ListDataEvent(this, ListDataEvent.INTERVAL_REMOVED, index, index)
        for (listener <- listeners){
            listener.intervalAdded(event)
        }
    }
    
    def remove(column:Int, row:Int){
        for((t,index) <- data.view.zipWithIndex){
            if (t.row == row && t.column == column){
                remove(index)
            }
        }
    }

    def getAll():List[Tile] = {
        data
    }

    override def addListDataListener(listener:ListDataListener){
        listeners = listeners :+ listener
    }

    override def getElementAt(index:Int) = {
        this.data.find(data.indexOf(_) == index) match {
            case Some(obj) => obj
            case None => throw new Exception("failed to find " + index)
        }
    }

    override def getSize():Int = {
        this.data.size
    }

    override def removeListDataListener(listener:ListDataListener){
        listeners = this.listeners diff List(listener)
    }
}

class TileSet(var name:String, var width:Int, var height:Int){
    var scrollX:Double = 0
    var scrollY:Double = 0
    
    var tileWidth:Int = 16
    var tileHeight:Int = 16
    
    var tiles = new TileListModel()
    
    var renderGrid:Boolean = true
    var gridColor:Color = new Color(255,255,255,30)
    
    var isCurrent:Boolean = false
    
    def render(g:Graphics2D, x:Int, y:Int, animations:AnimationListModel) = {
        if (renderGrid){
            var currentY = y
            while (currentY < (height + y)){
                var currentX = x
                while (currentX < (width + x)){
                    g.setColor(gridColor)
                    g.drawRect(currentX, currentY, tileWidth, tileHeight)
                    currentX += tileWidth
                }
                currentY += tileHeight
            }
        }
        
        tiles.getAll().foreach {
            case (tile) => {
                val animation = tile.animation
                if (animation != null){
                    animation.render(g, x + tile.column * tileWidth, y + tile.row * tileHeight)
                }
            }
        }
    }
    
    def readToken(token:Token, animations:AnimationListModel) = {
        if (!token.getName().equals("background") && !token.getName().equals("foreground")){
            throw new LoadException( "Starting token is not 'background' or 'foreground'" )
        }
        val nameToken = token.findToken("id")
        if (nameToken != null){
            name = nameToken.readString(0)
        }
        val typeToken = token.findToken("type")
        if (typeToken != null){
            if (typeToken.readString(0) != "tileset"){
                throw new LoadException("Not a tileset")
            }
        }
        val scrollxToken = token.findToken("scroll-x")
        if (scrollxToken != null){
            scrollX = scrollxToken.readDouble(0)
        }
        val scrollyToken = token.findToken("scroll-y")
        if (scrollyToken != null){
            scrollY = scrollyToken.readDouble(0)
        }
        val tilesetToken = token.findToken("tileset")
        if (tilesetToken != null){
            val sizeToken = tilesetToken.findToken("tile-size")
            if (sizeToken != null){
                tileWidth = sizeToken.readInt(0)
                tileHeight = sizeToken.readInt(1)
            }
            val dimensionToken = tilesetToken.findToken("dimensions")
            if (dimensionToken != null){
                width = dimensionToken.readInt(0) * tileWidth
                height = dimensionToken.readInt(1) * tileHeight
            }
            
            val tileIterator = tilesetToken.findTokens("tile").iterator()
            while(tileIterator.hasNext()){
                val t = tileIterator.next()
                val tile = new Tile()
                tile.readToken(t, animations)
                tiles.add(tile)
                val up = t.findToken("repeat-up")
                if (up != null){
                    var pos = tile.row
                    for (pos <- tile.row to (tile.row - up.readInt(0))){
                        val rep = new Tile()
                        rep.animation = tile.animation
                        rep.column = tile.column
                        rep.row = pos
                        tiles.add(rep)
                    }
                }
                val down = t.findToken("repeat-down")
                if (down != null){
                    var pos = tile.row
                    for (pos <- tile.row to (tile.row + down.readInt(0))){
                        val rep = new Tile()
                        rep.animation = tile.animation
                        rep.column = tile.column
                        rep.row = pos
                        tiles.add(rep)
                    }
                }
                val left = t.findToken("repeat-left")
                if (left != null){
                    var pos = tile.column
                    for (pos <- tile.column to (tile.column - left.readInt(0))){
                        val rep = new Tile()
                        rep.animation = tile.animation
                        rep.column = pos
                        rep.row = tile.row
                        tiles.add(rep)
                    }
                }
                val right = t.findToken("repeat-right")
                if (right != null){
                    var pos = tile.column
                    for (pos <- tile.column to (tile.column + right.readInt(0))){
                        val rep = new Tile()
                        rep.animation = tile.animation
                        rep.column = pos
                        rep.row = tile.row
                        tiles.add(rep)
                    }
                }
            }
        }
    }

    def toToken(position:String):Token = {
        val background = new Token()
        background.addToken(new Token(background, position))
        background.addToken(Array("id", name))
        background.addToken(Array("type", "tileset"))
        background.addToken(Array("scroll-x", String.valueOf(scrollX)))
        background.addToken(Array("scroll-y", String.valueOf(scrollY)))
        
        val tileset = new Token()
        tileset.addToken(new Token(tileset, "tileset"))
        tileset.addToken(Array("tile-size", String.valueOf(tileWidth), String.valueOf(tileHeight)))
        tileset.addToken(Array("dimensions", String.valueOf(width/tileWidth), String.valueOf(height/tileHeight)))
        
        tiles.getAll().foreach {
            case (tile) => tileset.addToken(tile.toToken())
        }
        
        background.addToken(tileset)
        
        background
    }
    
    override def toString():String = {
        var n = name
        if(isCurrent){
            n = n + " (currently selected)"
        }
        
        n
    }
    
    def editDialog(view:JPanel, viewScroll:JScrollPane, list:JList[TileSet], animations:List[Animation]) = {
        try {
            val engine = new SwingEngine("platformer/tileset.xml")
            val pane = engine.find("dialog").asInstanceOf[JDialog]
            
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
                        list.revalidate()
                        list.repaint()
                    }
                })
            }
            
            {
                val scroll = engine.find("scroll-x").asInstanceOf[JSpinner]
                scroll.setModel(new SpinnerNumberModel(scrollX, -2000.0, 2000.0, 0.001))
                scroll.addChangeListener(new ChangeListener(){
                    override def stateChanged(event:ChangeEvent){
                        val spinner = event.getSource().asInstanceOf[JSpinner]
                        val i = spinner.getValue().asInstanceOf[java.lang.Double]
                        scrollX = i.doubleValue()
                    }
                })
            }
            
            {
                val scroll = engine.find("scroll-y").asInstanceOf[JSpinner]
                scroll.setModel(new SpinnerNumberModel(scrollY, -2000.0, 2000.0, 0.001))
                scroll.addChangeListener(new ChangeListener(){
                    override def stateChanged(event:ChangeEvent){
                        val spinner = event.getSource().asInstanceOf[JSpinner]
                        val i = spinner.getValue().asInstanceOf[java.lang.Double]
                        scrollY = i.doubleValue()
                    }
                })
            }
            
            {
                val tilex = engine.find("tile-width").asInstanceOf[JSpinner]
                val model = new SpinnerNumberModel()
                tilex.setModel(model)
                model.setValue(tileWidth)
                tilex.addChangeListener(new ChangeListener(){
                    override def stateChanged(event:ChangeEvent){
                        val spinner = event.getSource().asInstanceOf[JSpinner]
                        val i = spinner.getValue().asInstanceOf[java.lang.Integer]
                        tileWidth = i.intValue()
                        view.revalidate()
                        viewScroll.repaint()
                    }
                })
            }
            
            {
                val tiley = engine.find("tile-height").asInstanceOf[JSpinner]
                val model = new SpinnerNumberModel()
                tiley.setModel(model)
                model.setValue(tileHeight)
                tiley.addChangeListener(new ChangeListener(){
                    override def stateChanged(event:ChangeEvent){
                        val spinner = event.getSource().asInstanceOf[JSpinner]
                        val i = spinner.getValue().asInstanceOf[java.lang.Integer]
                        tileHeight = i.intValue()
                        view.revalidate()
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
            
            // Tiles
            {
                val tileList = engine.find("tiles").asInstanceOf[JList[Tile]]
                tileList.setModel(tiles)
                
                val add = engine.find("add-tile-button").asInstanceOf[JButton]
                add.addActionListener(new ActionListener() { 
                    def actionPerformed(e:ActionEvent) = {
                        if (animations.size != 0){
                            val tile = new Tile()
                            tiles.add(tile)
                            tile.editDialog(view, viewScroll, tileList, animations)
                        } else {
                            JOptionPane.showMessageDialog(pane, "Please add some animations first.")
                        }
                    } 
                })
                
                val edit = engine.find("edit-tile-button").asInstanceOf[JButton]
                edit.addActionListener(new ActionListener() { 
                    def actionPerformed(e:ActionEvent) = {
                        if (tiles.getSize() > 0 && tileList.getSelectedIndex() != -1){
                            tiles.getElementAt(tileList.getSelectedIndex()).editDialog(view, viewScroll, tileList, animations)
                        }
                    } 
                })
                
                val remove = engine.find("remove-tile-button").asInstanceOf[JButton]
                remove.addActionListener(new ActionListener() { 
                    def actionPerformed(e:ActionEvent) = {
                        if (tiles.getSize() > 0 && tileList.getSelectedIndex() != -1){
                            tiles.remove(tileList.getSelectedIndex())
                            view.revalidate()
                            viewScroll.repaint()
                            list.updateUI()
                            list.revalidate()
                            list.repaint()
                        }
                    } 
                })
            }
            
            // Display grid?
            {
                val checkbox = engine.find("draw-grid").asInstanceOf[JCheckBox]
                checkbox.setSelected(renderGrid)
                checkbox.addActionListener(new ActionListener() {
                    def actionPerformed(e:ActionEvent) = {
                        renderGrid = checkbox.isSelected()
                        view.revalidate()
                        viewScroll.repaint()
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
                        g.setColor(gridColor)
                        g.fillRect(0, 0, this.getWidth(), this.getHeight())
                        g.setColor(Color.BLACK)
                        g.drawRect(0,0,this.getWidth()-1,this.getHeight()-1)
                    }
                }
                val colorPanel = engine.find("grid-color-display").asInstanceOf[JPanel]
                colorPanel.add(viewColor)
                
                val button = engine.find("grid-color").asInstanceOf[JButton]
                button.addActionListener(new ActionListener() {
                    def actionPerformed(e:ActionEvent) = {
                        val color = JColorChooser.showDialog(button, "Select a Fill Color", gridColor);
                        if (color != null){
                            gridColor = color
                            view.revalidate()
                            viewScroll.repaint()
                            viewColor.revalidate()
                            colorPanel.repaint()
                        }
                    }
                })
            }
            
            // Close
            {
                val close = engine.find("close").asInstanceOf[JButton]
                close.addActionListener(new ActionListener() { 
                    def actionPerformed(e:ActionEvent) = {
                        pane.setVisible(false)
                        view.revalidate()
                        viewScroll.repaint()
                        list.updateUI()
                        list.revalidate()
                        list.repaint()
                    } 
                })
            }
            
            // Show Dialog
            pane.repaint()
            pane.setModal(true)
            pane.setVisible(true)
        } catch {
            case e:Exception => JOptionPane.showMessageDialog(null, "error on opening, reason: " + e.getMessage(), "ERROR", JOptionPane.ERROR_MESSAGE)
        }
    }
}

class TileSetListModel extends ListModel[TileSet] {
    var data:List[TileSet] = List[TileSet]()
    var listeners = List[ListDataListener]()

    def add(tileset:TileSet){
        data = data :+ tileset
        val event = new ListDataEvent(this, ListDataEvent.INTERVAL_ADDED, data.size, data.size)
        for (listener <- listeners){
            listener.intervalAdded(event)
        }
    }
    
    def remove(index:Int){
        data = data.filterNot(data.indexOf(_) == index)
        val event = new ListDataEvent(this, ListDataEvent.INTERVAL_REMOVED, index, index)
        for (listener <- listeners){
            listener.intervalAdded(event)
        }
    }

    def getAll():List[TileSet] = {
        data
    }
    
    def swap(pos1:Int, pos2:Int) = {
        if (pos1 >= 0 && pos2 >= 0 && pos1 < data.size && pos2 < data.size){
            data = data.updated(pos1, data(pos2)).updated(pos2, data(pos1))
            val event = new ListDataEvent(this, ListDataEvent.CONTENTS_CHANGED, pos1, pos2)
            for (listener <- listeners){
                listener.intervalAdded(event)
            }
        }
    }

    override def addListDataListener(listener:ListDataListener){
        listeners = listeners :+ listener
    }

    override def getElementAt(index:Int) = {
        this.data.find(data.indexOf(_) == index) match {
            case Some(obj) => obj
            case None => throw new Exception("failed to find " + index)
        }
    }

    override def getSize():Int = {
        this.data.size
    }

    override def removeListDataListener(listener:ListDataListener){
        listeners = this.listeners diff List(listener)
    }
}
