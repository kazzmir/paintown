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
    var animation:Animation = _
    
    var row:Int = 0
    var column:Int = 0
    
    var x:Int = 0
    var y:Int = 0
    var width:Int = 0
    var height:Int = 0
    
    def toToken():Token = {
        val tile = new Token()
        tile.addToken(new Token(tile, "tile"))
        tile.addToken(Array("animation", String.valueOf(animation)))
        tile.addToken(Array("position", String.valueOf(row), String.valueOf(column)))
        
        tile
    }
    
    override def toString:String = {
        toToken().toString()
    }
    
    def editDialog(view:JPanel, viewScroll:JScrollPane, list:JList[Tile]) = {
        try {
            // Show Dialog
            /*pane.repaint()
            pane.setModal(true)
            pane.setVisible(true)*/
        } catch {
            case e:Exception => JOptionPane.showMessageDialog(null, "error on opening, reason: " + e.getMessage(), "ERROR", JOptionPane.ERROR_MESSAGE)
        }
    }
}

class TileListModel extends ListModel[Tile] {
    var data:List[Tile] = List[Tile]()
    var listeners = List[ListDataListener]()

    def add(tile:Tile){
        data = data :+ tile
        val event = new ListDataEvent(this, ListDataEvent.INTERVAL_ADDED, data.size, data.size)
        for (listener <- listeners){
            listener.intervalAdded(event)
        }
    }
    
    def remove(index:Int){
        data = data.remove(data.indexOf(_) == index)
        val event = new ListDataEvent(this, ListDataEvent.INTERVAL_REMOVED, index, index)
        for (listener <- listeners){
            listener.intervalAdded(event)
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
        listeners = this.listeners - listener
    }
}

class TileSet(n:String){
    
    var name:String = n
    var scrollX:Double = 0
    var scrollY:Double = 0
    
    var width:Int = 640
    var height:Int = 480
    
    var tileWidth:Int = 16
    var tileHeight:Int = 16
    
    var tiles = new TileListModel()
    
    var renderGrid:Boolean = true
    
    var isCurrent:Boolean = false
    
    def render(g:Graphics2D, x:Int, y:Int) = {
        if (renderGrid){
            var currentY = y
            while (currentY < (height + y)){
                var currentX = x
                while (currentX < (width + x)){
                    g.setColor( new Color( 255, 255, 255 ) )
                    g.drawRect(currentX, currentY, tileWidth, tileHeight)
                    currentX += tileWidth
                }
                currentY += tileHeight
            }
        }
    }

    def toToken():Token = {
        val tileset = new Token()
        tileset.addToken(new Token(tileset, "tileset"))
        tileset.addToken(Array("tile-size", String.valueOf(tileWidth), String.valueOf(tileHeight)))
        
        tiles.getAll().foreach {
            case (tile) => tileset.addToken(tile.toToken())
        }
        
        tileset
    }
    
    override def toString():String = {
        var n = name
        if(isCurrent){
            n = n + " (currently selected)"
        }
        
        n
    }
    
    def editDialog(view:JPanel, viewScroll:JScrollPane, list:JList[TileSet]) = {
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
                        val tile = new Tile()
                        tiles.add(tile)
                        tile.editDialog(view, viewScroll, tileList)
                    } 
                })
                
                val edit = engine.find("edit-tile-button").asInstanceOf[JButton]
                edit.addActionListener(new ActionListener() { 
                    def actionPerformed(e:ActionEvent) = {
                        if (tiles.getSize() > 0 && tileList.getSelectedIndex() != -1){
                            tiles.getElementAt(tileList.getSelectedIndex()).editDialog(view, viewScroll, tileList)
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
        data = data.remove(data.indexOf(_) == index)
        val event = new ListDataEvent(this, ListDataEvent.INTERVAL_REMOVED, index, index)
        for (listener <- listeners){
            listener.intervalAdded(event)
        }
    }

    def getAll():List[TileSet] = {
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
        listeners = this.listeners - listener
    }
}
