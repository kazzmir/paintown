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

class ImageHolder(i:Image, f:File){
    val image:Image = i
    val file:File = f
    
    def render(g:Graphics2D, x:Int, y:Int) = {
        g.drawImage(image, x, y, null)
    }
}

class Frame{
    var image:Int = -1
    var alpha:Int = -1
    var hflip:Boolean = false
    var vflip:Boolean = false 
    var time:Int = 0

    def toToken():Token = {
        val frame = new Token()
        frame.addToken(new Token(frame, "frame"))
        frame.addToken(Array("image", String.valueOf(image)))
        if (alpha != -1){
            frame.addToken(Array("alpha", String.valueOf(alpha)))
        }
        frame.addToken(Array("hflip", String.valueOf(if(hflip) 1 else 0)))
        frame.addToken(Array("vflip", String.valueOf(if(vflip) 1 else 0)))
        frame.addToken(Array("time", String.valueOf(time)))
        frame
    }
}

class Animation(n:String){
    var name:String = n
    var basedir:File = _
    var images = scala.collection.mutable.HashMap.empty[Int, ImageHolder]
    var frames = scala.collection.mutable.ArrayBuffer.empty[Frame]
    var loop:Int = 0
    
    def render(g:Graphics2D, x:Int, y:Int) = {
        // Render
        if (frames.size > 0){
            // Get first
            var frame = frames(0)
            if (frame.image != -1){
                images(frame.image).render(g, x, y)
            }
        }
    }
    
    def loadImage(s:String):Image = {
        try{
            MaskedImage.load( MapEditor.getDataPath( s ).getPath() )
        } catch {
            case ex:IOException => throw new LoadException( "Could not load " + s )
        }
    }

    def toToken():Token = {
        val animation = new Token()
        animation.addToken(new Token(animation, "animation"))
        animation.addToken(Array("id", name))
        animation.addToken(Array("basedir", basedir.getPath()))
        images.foreach {
            case (key, image) => animation.addToken(Array("image", String.valueOf(key), image.file.getPath()))
        }
        frames.foreach {
            case (frame) => animation.addToken(frame.toToken())
        }
        animation.addToken(Array("loop", String.valueOf(loop)))

        animation
    }
    
    override def toString():String = {
        name
    }
    
    def editDialog(view:JPanel, viewScroll:JScrollPane, list:JList[Animation]) = {
        try {
            val engine = new SwingEngine("platformer/animation.xml")
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
                val loopField = engine.find("loop").asInstanceOf[JSpinner]
                val model = new SpinnerNumberModel()
                loopField.setModel(model)
                model.setValue(loop)
                loopField.addChangeListener(new ChangeListener(){
                    override def stateChanged(event:ChangeEvent){
                        val spinner = event.getSource().asInstanceOf[JSpinner]
                        val i = spinner.getValue().asInstanceOf[java.lang.Integer]
                        loop = i.intValue()
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
            pane.setVisible(true)
        } catch {
            case e:Exception => JOptionPane.showMessageDialog(null, "error on opening, reason: " + e.getMessage(), "ERROR", JOptionPane.ERROR_MESSAGE)
        }
    }
}

class AnimationListModel extends ListModel[Animation] {
    var data:List[Animation] = List[Animation]()
    var listeners = List[ListDataListener]()

    def add(anim:Animation){
        data = data :+ anim
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

    def getAll():List[Animation] = {
        data
    }
    
    def get(name:String):Animation = {
        data.foreach(anim => if (anim.name == name) anim)
        null
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
