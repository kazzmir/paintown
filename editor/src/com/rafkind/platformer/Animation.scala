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

class ImageHolder(f:File){
    val image:Image = loadImage(f)
    val file:File = f
    
    def render(g:Graphics2D, x:Int, y:Int) = {
        g.drawImage(image, x, y, null)
    }
    
    def renderScaled(g:Graphics2D, x:Int, y:Int) = {
        g.scale(2,2)
        render(g,x,y)
    }
    
    def loadImage(f:File):Image = { //s:String):Image = {
        try{
            MaskedImage.load( f.getPath() )//MapEditor.getDataPath( s ).getPath() )
        } catch {
            case ex:IOException => throw new LoadException( "Could not load " + f.getPath() )
        }
    }
    
    override def toString():String = {
        file.getPath()
    }
}

class ImageHolderListModel extends ListModel[ImageHolder] {
    var data:List[ImageHolder] = List[ImageHolder]()
    var listeners = List[ListDataListener]()

    def add(image:ImageHolder){
        data = data :+ image
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

    def getAll():List[ImageHolder] = {
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
    
    override def toString():String = {
        toToken().toString()
    }
    
    def editDialog(view:JPanel, viewScroll:JScrollPane, list:JList[Frame], images:List[ImageHolder]) = {
        try {
            val engine = new SwingEngine("platformer/frame.xml")
            val pane = engine.find("dialog").asInstanceOf[JDialog]
            
            {
                val imageField = engine.find("image").asInstanceOf[JComboBox[ImageHolder]]
                imageField.setModel(new DefaultComboBoxModel(images.toArray))
                
                val imageScroll = engine.find("image-view").asInstanceOf[JScrollPane]
                val imageView = new JPanel(){
                    override def getPreferredSize():Dimension = {
                        new Dimension(250, 250)
                    }

                    override def paintComponent(g:Graphics){
                        val h = viewScroll.getHorizontalScrollBar()
                        val v = viewScroll.getVerticalScrollBar()
                        g.setColor(new Color(64, 64, 64))
                        g.fillRect(0, 0, this.getWidth(), this.getHeight())
                        if (imageField.getSelectedIndex() != -1){
                            var image = images(imageField.getSelectedIndex())
                            image.renderScaled(g.asInstanceOf[Graphics2D], 0, 0)
                        }
                    }
                }
                
                imageField.addActionListener(new ActionListener() { 
                    def actionPerformed(e:ActionEvent) = {
                        image = imageField.getSelectedIndex()
                        list.revalidate()
                        list.repaint()
                        imageView.revalidate()
                        imageView.repaint()
                    } 
                })
                
                imageScroll.setViewportView(imageView)
                imageView.revalidate()
            }
            
            {
                val alphaField = engine.find("alpha").asInstanceOf[JSpinner]
                val model = new SpinnerNumberModel()
                alphaField.setModel(model)
                model.setValue(alpha)
                alphaField.addChangeListener(new ChangeListener(){
                    override def stateChanged(event:ChangeEvent){
                        val spinner = event.getSource().asInstanceOf[JSpinner]
                        val i = spinner.getValue().asInstanceOf[java.lang.Integer]
                        alpha = i.intValue()
                        list.revalidate()
                        list.repaint()
                    }
                })
            }
            
            {
                val timeField = engine.find("time").asInstanceOf[JSpinner]
                val model = new SpinnerNumberModel()
                timeField.setModel(model)
                model.setValue(time)
                timeField.addChangeListener(new ChangeListener(){
                    override def stateChanged(event:ChangeEvent){
                        val spinner = event.getSource().asInstanceOf[JSpinner]
                        val i = spinner.getValue().asInstanceOf[java.lang.Integer]
                        time = i.intValue()
                        list.revalidate()
                        list.repaint()
                    }
                })
            }
            
            {
                val vflipCheck = engine.find("vflip").asInstanceOf[JCheckBox]
                vflipCheck.setSelected(vflip)
                vflipCheck.addChangeListener(new ChangeListener(){
                    override def stateChanged(event:ChangeEvent){
                        val checkbox = event.getSource().asInstanceOf[JCheckBox]
                        val i = checkbox.isSelected().asInstanceOf[java.lang.Boolean]
                        vflip = i.booleanValue()
                        list.revalidate()
                        list.repaint()
                    }
                })
            }
            
            {
                val hflipCheck = engine.find("hflip").asInstanceOf[JCheckBox]
                hflipCheck.setSelected(hflip)
                hflipCheck.addChangeListener(new ChangeListener(){
                    override def stateChanged(event:ChangeEvent){
                        val checkbox = event.getSource().asInstanceOf[JCheckBox]
                        val i = checkbox.isSelected().asInstanceOf[java.lang.Boolean]
                        hflip = i.booleanValue()
                        list.revalidate()
                        list.repaint()
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

class FrameListModel extends ListModel[Frame] {
    var data:List[Frame] = List[Frame]()
    var listeners = List[ListDataListener]()

    def add(frame:Frame){
        data = data :+ frame
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

    def getAll():List[Frame] = {
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

class Animation(n:String){
    var name:String = n
    var basedir:File = _
    var images:ImageHolderListModel = new ImageHolderListModel()
    var frames:FrameListModel = new FrameListModel()
    var loop:Int = 0
    
    def render(g:Graphics2D, x:Int, y:Int) = {
        // Render
        if (frames.getSize() > 0){
            // Get first
            var frame = frames.getElementAt(0)
            if (frame.image != -1){
                images.getElementAt(frame.image).render(g, x, y)
            }
        }
    }

    def toToken():Token = {
        val animation = new Token()
        animation.addToken(new Token(animation, "animation"))
        animation.addToken(Array("id", name))
        animation.addToken(Array("basedir", basedir.getPath()))
        var index = 0
        images.getAll().foreach {
            case (image) => {
                animation.addToken(Array("image", String.valueOf(index), image.file.getPath()))
                index += 1
            }
        }
        frames.getAll().foreach {
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
                var dirField = engine.find("basedir").asInstanceOf[JTextField]
                val dir = engine.find("basedir-button").asInstanceOf[JButton]
                dir.addActionListener(new ActionListener() { 
                    def actionPerformed(e:ActionEvent) = {
                        val chooser = new JFileChooser(new File("."))	
                        chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY)
                        val returnVal = chooser.showOpenDialog(pane)
                        if (returnVal == JFileChooser.APPROVE_OPTION){
                          val newPath = chooser.getSelectedFile()
                          dirField.setText(newPath.getPath())
                          basedir = newPath
                        }
                    } 
                })
            }
            
            // Images
            {
                val imageList = engine.find("images").asInstanceOf[JList[ImageHolder]]
                imageList.setModel(images)
                
                val add = engine.find("add-image-button").asInstanceOf[JButton]
                add.addActionListener(new ActionListener() { 
                    def actionPerformed(e:ActionEvent) = {
                        val chooser = new JFileChooser(new File("."))	
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
                          images.add(new ImageHolder(chooser.getSelectedFile()))
                        }
                    } 
                })
                
                val remove = engine.find("remove-image-button").asInstanceOf[JButton]
                remove.addActionListener(new ActionListener() { 
                    def actionPerformed(e:ActionEvent) = {
                        if (images.getSize() > 0 && imageList.getSelectedIndex() != -1){
                            images.remove(imageList.getSelectedIndex())
                        }
                    } 
                })
            }
            
            // Frames
            {
                val frameList = engine.find("frames").asInstanceOf[JList[Frame]]
                frameList.setModel(frames)
                
                val add = engine.find("add-frame-button").asInstanceOf[JButton]
                add.addActionListener(new ActionListener() { 
                    def actionPerformed(e:ActionEvent) = {
                        val frame = new Frame()
                        frames.add(frame)
                        frame.editDialog(view, viewScroll, frameList, images.getAll())
                    } 
                })
                
                val edit = engine.find("edit-frame-button").asInstanceOf[JButton]
                edit.addActionListener(new ActionListener() { 
                    def actionPerformed(e:ActionEvent) = {
                        if (frames.getSize() > 0 && frameList.getSelectedIndex() != -1){
                            frames.getElementAt(frameList.getSelectedIndex()).editDialog(view, viewScroll, frameList, images.getAll())
                        }
                    } 
                })
                
                val remove = engine.find("remove-frame-button").asInstanceOf[JButton]
                remove.addActionListener(new ActionListener() { 
                    def actionPerformed(e:ActionEvent) = {
                        if (frames.getSize() > 0 && frameList.getSelectedIndex() != -1){
                            frames.remove(frameList.getSelectedIndex())
                            view.revalidate()
                            viewScroll.repaint()
                        }
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
            pane.setModal(true)
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
