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

class Area(var name:String){
    var x:Int = 0
    var y:Int = 0
    var width:Int = 0
    var height:Int = 0
    var color:Color = new Color(204,0,51)
    var font = new Font("Verdana", Font.BOLD, 4)
    
    def render(g:Graphics2D, x1:Int, y1:Int) = {
        g.setColor(color)
        g.drawRect(x1 + x, y1 + y, width, height)
        g.setFont(font);
        g.drawString( "Collision map [" + name + "]", x1 + x -1, y1 + y -1)
    }
    
    def readToken(token:Token) = {
        if (!token.getName().equals("area")){
            throw new LoadException( "Starting token is not 'area'" )
        }
        val nameToken = token.findToken("id")
        if (nameToken != null){
            name = nameToken.readString(0)
        }
        
        val positionToken = token.findToken("position")
        if (positionToken != null){
            x = positionToken.readInt(0)
            y = positionToken.readInt(1)
            width = positionToken.readInt(2)
            height = positionToken.readInt(3)
        }
    }
    
    def toToken():Token = {
        val area = new Token()
        area.addToken(new Token(area, "area "))
        area.addToken(Array("id", name))
        area.addToken(Array("position", String.valueOf(x), String.valueOf(y), String.valueOf(width), String.valueOf(height)))
        
        area
    }
    
    override def toString():String = {
        toToken().toString()
    }
    
    def editDialog(view:JPanel, viewScroll:JScrollPane, list:JList[Area]) = {
        try {
            val engine = new SwingEngine("platformer/area.xml")
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
            // x
            {
                val xspin = engine.find("x").asInstanceOf[JSpinner]
                val model = new SpinnerNumberModel()
                xspin.setModel(model)
                model.setValue(x)
                xspin.addChangeListener(new ChangeListener(){
                    override def stateChanged(event:ChangeEvent){
                        val spinner = event.getSource().asInstanceOf[JSpinner]
                        val i = spinner.getValue().asInstanceOf[java.lang.Integer]
                        x = i.intValue()
                        view.revalidate()
                        viewScroll.repaint()
                    }
                })
            }
            
            // y
            {
                val yspin = engine.find("y").asInstanceOf[JSpinner]
                val model = new SpinnerNumberModel()
                yspin.setModel(model)
                model.setValue(y)
                yspin.addChangeListener(new ChangeListener(){
                    override def stateChanged(event:ChangeEvent){
                        val spinner = event.getSource().asInstanceOf[JSpinner]
                        val i = spinner.getValue().asInstanceOf[java.lang.Integer]
                        y = i.intValue()
                        view.revalidate()
                        viewScroll.repaint()
                    }
                })
            }
            
            // width
            {
                val widthspin = engine.find("width").asInstanceOf[JSpinner]
                val model = new SpinnerNumberModel()
                widthspin.setModel(model)
                model.setValue(width)
                widthspin.addChangeListener(new ChangeListener(){
                    override def stateChanged(event:ChangeEvent){
                        val spinner = event.getSource().asInstanceOf[JSpinner]
                        val i = spinner.getValue().asInstanceOf[java.lang.Integer]
                        width = i.intValue()
                        view.revalidate()
                        viewScroll.repaint()
                    }
                })
            }
            
            // height
            {
                val heightspin = engine.find("height").asInstanceOf[JSpinner]
                val model = new SpinnerNumberModel()
                heightspin.setModel(model)
                model.setValue(height)
                heightspin.addChangeListener(new ChangeListener(){
                    override def stateChanged(event:ChangeEvent){
                        val spinner = event.getSource().asInstanceOf[JSpinner]
                        val i = spinner.getValue().asInstanceOf[java.lang.Integer]
                        height = i.intValue()
                        view.revalidate()
                        viewScroll.repaint()
                    }
                })
            }
            
            // Color
            {
                val viewColor = new JPanel(){
                    override def getPreferredSize():Dimension = {
                        new Dimension(25,25)
                    }

                    override def paintComponent(g:Graphics){
                        g.setColor(color)
                        g.fillRect(0, 0, this.getWidth(), this.getHeight())
                    }
                }
                val colorPanel = engine.find("color-display").asInstanceOf[JPanel]
                colorPanel.add(viewColor)
                
                val button = engine.find("color").asInstanceOf[JButton]
                button.addActionListener(new ActionListener() {
                    def actionPerformed(e:ActionEvent) = {
                        val chosen = JColorChooser.showDialog(button, "Select a Fill Color", color);
                        if (chosen != null){
                            color = chosen
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

class CollisionMap extends ListModel[Area] {
    var data:List[Area] = List[Area]()
    var listeners = List[ListDataListener]()

    def add(area:Area){
        data = data :+ area
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

    def getAll():List[Area] = {
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
    
    def render(g:Graphics2D, x:Int, y:Int) = {
        data.foreach {
            case (area) => area.render(g,x,y)
        }
    }
    
    def readToken(token:Token) = {
        if (!token.getName().equals("collision-map")){
            throw new LoadException( "Starting token is not 'collision-map'" )
        }
        
        val areaIterator = token.findTokens("area").iterator()
        while(areaIterator.hasNext()){
            val area = new Area("Area")
            area.readToken(areaIterator.next())
            add(area)
        }
    }

    def toToken():Token = {
        val collisionMap = new Token()
        collisionMap.addToken(new Token(collisionMap, "collision-map"))
        
        data.foreach {
            case (area) => collisionMap.addToken(area.toToken())
        }
        
        collisionMap
    }
}
