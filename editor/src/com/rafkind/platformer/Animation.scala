package com.rafkind.platformer

import java.awt._
import java.awt.image._
import javax.swing._
import java.io._
import java.net.URI
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

class ImageHolder(val basedir:File, val file:File){

    def this() = this(null,null)
    
    var image:Image = null
    if (basedir != null && file != null){
        image = loadImage()
    }
    
    def render(g:Graphics2D, x:Int, y:Int) = {
        if (image != null){
            g.drawImage(image, x, y, null)
        }
    }
    
    def renderScaled(g:Graphics2D, x:Int, y:Int, width:Int, height:Int) = {
        if (image != null){
            g.drawImage(image.getScaledInstance(width,height,Image.SCALE_SMOOTH), x, y, null)
        }
    }
    
    def loadImage():Image = {
        val path = MapEditor.getDataPath(basedir.getPath() + "/" + file.getPath())
        try{
            MaskedImage.load(path.getPath())
        } catch {
            case ex:IOException => throw new LoadException( "Could not load " + path.getPath() )
        }
    }
    
    def getWidth():Int = {
        if (image != null){
            return image.getWidth(null)
        }
        0
    }
    
    def getHeight():Int = {
        if (image != null){
            return image.getHeight(null)
        }
        0
    }
    
    override def toString():String = {
        if (file != null){
            return file.getPath()
        }
        "None"
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
    var alpha:Int = 255
    var hflip:Boolean = false
    var vflip:Boolean = false 
    var time:Int = 0
    
    def readToken(token:Token) = {
        if (!token.getName().equals("frame")){
            throw new LoadException( "Starting token is not 'frame'" )
        }
        val imageToken = token.findToken("image")
        if (imageToken != null){
            image = imageToken.readInt(0)
        }
        val alphaToken = token.findToken("alpha")
        if (alphaToken != null){
            alpha = alphaToken.readInt(0)
        }
        val hflipToken = token.findToken("hflip")
        if (hflipToken != null){
            hflip = hflipToken.readBoolean(0)
        }
        val vflipToken = token.findToken("vflip")
        if (vflipToken != null){
            vflip = vflipToken.readBoolean(0)
        }
        val timeToken = token.findToken("time")
        if (timeToken != null){
            time = timeToken.readInt(0)
        }
    }

    def toToken():Token = {
        val frame = new Token()
        frame.addToken(new Token(frame, "frame"))
        frame.addToken(Array("image", String.valueOf(image)))
        frame.addToken(Array("alpha", String.valueOf(alpha)))
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
                val imageArray = images.toArray :+ new ImageHolder()
                imageField.setModel(new DefaultComboBoxModel(imageArray))
                
                imageField.setSelectedIndex(image)
                
                val imagePanel = engine.find("image-view").asInstanceOf[JPanel]
                val imageView = new JPanel(){
                    override def getPreferredSize():Dimension = {
                        new Dimension(150, 150)
                    }

                    override def paintComponent(g:Graphics){
                        val h = viewScroll.getHorizontalScrollBar()
                        val v = viewScroll.getVerticalScrollBar()
                        g.setColor(new Color(64, 64, 64))
                        g.fillRect(0, 0, this.getWidth(), this.getHeight())
                        if (image != -1){
                            var img = images(image)
                            img.renderScaled(g.asInstanceOf[Graphics2D], 50, 50, 49, 49)
                        }
                    }
                }
                
                imageField.addActionListener(new ActionListener() { 
                    def actionPerformed(e:ActionEvent) = {
                        val index = imageField.getSelectedIndex()
                        if (imageArray(index).toString() == "None"){
                            image = -1
                        } else {
                            image = index
                        }
                        list.revalidate()
                        list.repaint()
                        imageView.revalidate()
                        imageView.repaint()
                    } 
                })
                
                imagePanel.add(imageView)
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
        listeners = this.listeners - listener
    }
}

class Animation(var name:String){
    var basedir:File = null
    var images:ImageHolderListModel = new ImageHolderListModel()
    var frames:FrameListModel = new FrameListModel()
    var loop:Int = 0
    var isCurrent:Boolean = false
    var index:Int = 0
    var currentTime:Int = 0
    
    def render(g:Graphics2D, x:Int, y:Int) = {
        // Render
        if (frames.getSize() > 0){
            var frame = frames.getElementAt(index)
            if (frame.image != -1){
                images.getElementAt(frame.image).render(g, x, y)
            }
        }
    }
    
    def renderScaled(g:Graphics2D, x:Int, y:Int, width:Int, height:Int) = {
        if (frames.getSize() > 0){
            var frame = frames.getElementAt(index)
            if (frame.image != -1){
                images.getElementAt(frame.image).renderScaled(g, x, y, width, height)
            }
        }
    }
    
    def update() = {
        if (frames.getSize() > 0){
            currentTime = currentTime + 1
            if (currentTime >= frames.getElementAt(index).time){
                currentTime = 0
                next()
            }
        }
    }
    
    def next() = {
        if (index < frames.getSize()-1){
            index = index + 1
        } else {
            index = loop
        }
    }
    
    def readToken(token:Token) = {
        if (!token.getName().equals("animation")){
            throw new LoadException( "Starting token is not 'animation'" )
        }
        val nameToken = token.findToken("id")
        if (nameToken != null){
            name = nameToken.readString(0)
        }
        val basedirToken = token.findToken("basedir")
        if (basedirToken != null){
            basedir = new File(basedirToken.readString(0))
        }
        val imageIterator = token.findTokens("image").iterator()
        while(imageIterator.hasNext()){
            val t = imageIterator.next()
            images.add(new ImageHolder(basedir, new File(t.readString(1))))
        }
        val frameIterator = token.findTokens("frame").iterator()
        while(frameIterator.hasNext()){
            val frame = new Frame()
            frame.readToken(frameIterator.next())
            frames.add(frame)
        }
    }

    def toToken():Token = {
        val animation = new Token()
        animation.addToken(new Token(animation, "animation"))
        animation.addToken(Array("id", name))
        animation.addToken(Array("basedir", basedir.getPath() + "/"))
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
        var n = name
        if(isCurrent){
            n = n + " (currently selected)"
        }
        
        n
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
                dirField.setEditable(false)
                if (basedir != null){
                    dirField.setText(basedir.getPath())
                }
                val dir = engine.find("basedir-button").asInstanceOf[JButton]
                dir.addActionListener(new ActionListener() { 
                    def actionPerformed(e:ActionEvent) = {
                        if (MapEditor.getDataPath("/").isDirectory()){
                            val chooser = new JFileChooser(MapEditor.getDataPath("/"))
                            chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY)
                            val returnVal = chooser.showOpenDialog(pane)
                            if (returnVal == JFileChooser.APPROVE_OPTION){
                            val choosen:File = chooser.getSelectedFile()
                            // Doesn't work scala complains
                            /*val base:URI = URI.create(MapEditor.getDataPath(".").getPath())
                            val abs:URI = URI.create(choosen.getPath())
                            basedir = base.relativize(abs)*/
                            val base = MapEditor.getDataPath("/").getPath()
                            val absolute = choosen.getPath()
                            val relative = absolute.replace(base,"")
                            basedir = new File(relative)
                            dirField.setText(basedir.getPath())
                            }
                        } else {
                            JOptionPane.showMessageDialog(pane, "Please set the data directory in the menu.")
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
                        if (basedir != null && MapEditor.getDataPath(basedir.getPath()).isDirectory()){
                            val chooser = new JFileChooser(MapEditor.getDataPath(basedir.getPath()))
                            chooser.setFileFilter(new FileFilter(){
                                def accept(f:File):Boolean = {
                                    f.isDirectory() || f.getName().endsWith( ".png" )
                                }

                                def getDescription():String = {
                                    "Png files"
                                }
                            })
                            chooser.setMultiSelectionEnabled(true)
                            val returnVal = chooser.showOpenDialog(pane)
                            if (returnVal == JFileChooser.APPROVE_OPTION){
                                chooser.getSelectedFiles().foreach{
                                    case (file) => images.add(new ImageHolder(basedir, new File(file.getName())))
                                }
                            }
                        } else {
                            JOptionPane.showMessageDialog(pane, "Please select a base directory.")
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
                        if (images.getSize() != 0){
                            val frame = new Frame()
                            frames.add(frame)
                            frame.editDialog(view, viewScroll, frameList, images.getAll())
                        } else {
                            JOptionPane.showMessageDialog(pane, "Please add some images first.")
                        }
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
                
                val up = engine.find("up-frame-button").asInstanceOf[JButton]
                up.addActionListener(new ActionListener() { 
                    def actionPerformed(e:ActionEvent) = {
                        if (frames.getSize() > 0 && frameList.getSelectedIndex() != -1){
                            var index = frameList.getSelectedIndex()
                            frames.swap(index, index-1)
                            index = index-1
                            frameList.setSelectedIndex(index)
                            frameList.updateUI()
                        }
                    } 
                })
                
                val down = engine.find("down-frame-button").asInstanceOf[JButton]
                down.addActionListener(new ActionListener() { 
                    def actionPerformed(e:ActionEvent) = {
                        if (frames.getSize() > 0 && frameList.getSelectedIndex() != -1){
                            var index = frameList.getSelectedIndex()
                            frames.swap(index, index+1)
                            index = index+1
                            frameList.setSelectedIndex(index)
                            frameList.updateUI()
                        }
                    } 
                })
                
                val loopButton = engine.find("loop-frame-button").asInstanceOf[JButton]
                loopButton.addActionListener(new ActionListener() { 
                    def actionPerformed(e:ActionEvent) = {
                        if (frames.getSize() > 0 && frameList.getSelectedIndex() != -1){
                            loop = frameList.getSelectedIndex()
                            val loopField = engine.find("loop").asInstanceOf[JTextField]
                            loopField.setText("Loop at frame (" + String.valueOf(loop) + ")")
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
                val loopField = engine.find("loop").asInstanceOf[JTextField]
                loopField.setText("Loop at frame (" + String.valueOf(loop) + ")")
                loopField.setEditable(false)
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
        data.foreach {
            case(anim) => {
                if (anim.name == name){
                    // weird that I have to use a return keyword
                    return anim
                }
            }
        }
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
